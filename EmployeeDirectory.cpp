#include <stdint.h>
#include <string>
#include <optional>
#include <tuple>
#include <vector>
#include <unordered_map>

class Employee
{
public:
  const uint32_t m_empId;
  const std::string m_name;
  const uint32_t m_phoneNo;
  
    Employee(const uint32_t empId,
             const std::string name,
             const uint32_t phoneNo):
        m_empId(empId),
        m_name(name),
        m_phoneNo(phoneNo)
    {}

    Employee(const Employee& other) = default;
};

class EmployeeDirectory
{
  typedef std::tuple <Employee, std::optional<Employee>, std::vector<Employee>> EmployeeDetails;
  typedef std::unordered_map<uint32_t, Employee> Employees;
  Employees m_employees;
  std::unordered_map<uint32_t, std::vector<Employees::iterator>> m_managers;
  std::unordered_map<uint32_t, Employees::iterator> m_subordinates;

  const uint32_t m_ceoId;
  public:
  EmployeeDirectory(const uint32_t& ceoEmpId,
                    const std::string& ceoName,
                    const uint32_t& ceoPhoneNo):
    m_ceoId(ceoEmpId)
  {
    m_employees.emplace(ceoEmpId, Employee(ceoEmpId, ceoName, ceoPhoneNo));
  }

  bool addEmployee(const uint32_t& empId,
                   const std::string& name,
                   const uint32_t& phoneNo,
                   const uint32_t managerId)
  {
    if (m_employees.find(empId) == m_employees.end() &&
        m_employees.find(managerId) != m_employees.end())
    {
      return false;
    }
    
    m_managers[managerId].push_back(m_employees.emplace(empId, Employee(empId, name, phoneNo)).first);
    m_subordinates[empId] = m_employees.find(managerId);
    return true;
  }

  bool removeEmployee(const uint32_t& empId)
  {
    if (empId == m_ceoId)
    {
      return false;
    }
    else if (auto it = m_employees.find(empId); it == m_employees.end())
    {
      return false;
    }
    else
    {
      if (auto itManager = m_managers.find(empId); itManager != m_managers.end())
      {
        auto thisEmployeesManager = m_subordinates.find(empId);
        for (auto const& subordinate : itManager->second)
        {
          m_subordinates[subordinate->first] = thisEmployeesManager->second;
        }

        m_managers.erase(itManager);
      }

      m_employees.erase(it);
      return true;
    }
  }

  bool addSubordinate(const uint32_t &empId, const uint32_t& subordinateId)
  {
    if (m_employees.find(empId) == m_employees.end())
    {
      return false;
    }
    else if (m_employees.find(subordinateId) == m_employees.end())
    {
      return false;
    }
    else if(m_subordinates.find(subordinateId) != m_subordinates.end())
    {
      return false;
    }
    else
    {
      m_managers[empId].push_back(m_employees.find(subordinateId));
      m_subordinates.emplace(subordinateId, m_employees.find(empId));
      return true;
    }
  }

  std::optional<EmployeeDetails> queryEmployee(const uint32_t& empId)
  {
    std::optional<EmployeeDetails> res;
    if (auto it = m_employees.find(empId); it != m_employees.end())
    {
      std::vector<Employee> subordinates;
      std::optional<Employee> boss;
      if (auto itBoss = m_subordinates.find(empId); itBoss != m_subordinates.end())
      {
        boss.emplace(itBoss->second->second);
      }

      if (auto itSubordinates = m_managers.find(empId); itSubordinates != m_managers.end())
      {
        for (auto const& itSubordinate : itSubordinates->second)
        {
          subordinates.push_back(itSubordinate->second);
        }
      }

      res.emplace(std::make_tuple(it->second, boss, subordinates));
    }

    return res;
  }
};

#include <iostream>
#include <string.h>

int main()
{
  int n;
  uint32_t bossId;
  char bossName[128];
  uint32_t bossPhNo;

  std::cin >> n >> bossId >> bossName >> bossPhNo;

  EmployeeDirectory employeeDirectory({bossId, bossName, bossPhNo});

  char lineInput[1024];
  for (int i = 0; i < n; ++i)
  {
    std::cin.getline(lineInput, 1023);
    char operation = lineInput[0];
    
    uint32_t empId;
    char empName[128];
    uint32_t phNo;
    switch (operation)
    {
    case 'A':
      sscanf(lineInput  + 1, "%u %s %u", &empId, empName, &phNo);      
      std::cout << employeeDirectory.addEmployee(empId, empName, phNo, bossId);
      break;

    default:
      break;
    }
  }
  return 0;
}