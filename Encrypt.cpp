#include <iostream>
#include <fstream>
#include <string>

// Simple checksum: XOR all characters in the key
char key_checksum(const std::string &key)
{
  char sum = 0;
  for (char c : key)
    sum ^= c;
  return sum;
}

std::string encrypt(std::string text, std::string key)
{
  std::string result = text;
  for (size_t i = 0; i < text.length(); ++i)
  {
    result[i] = text[i] ^ key[i % key.length()];
  }
  return result;
}

int main(int argc, char **argv)
{
  if (argc < 2)
  {
    std::cerr << "Usage:\n";
    std::cerr << "  " << argv[0] << " -e <sourceFile> <targetFile>\n";
    std::cerr << "  " << argv[0] << " -d <encryptedFile>\n";
    return 1;
  }

  std::string mode = argv[1];

  if (mode == "-e")
  {
    if (argc < 4)
    {
      std::cerr << "Missing arguments for encryption.\n";
      return 1;
    }
    std::string sourceFile = argv[2];
    std::string targetFile = argv[3];

    std::ifstream inFile(sourceFile, std::ios::binary);
    if (!inFile)
    {
      std::cerr << "Failed to open source file.\n";
      return 1;
    }
    std::string text((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
    inFile.close();

    std::string key;
    std::cout << "Enter encryption key: ";
    std::getline(std::cin, key);

    char checksum = key_checksum(key);
    std::string encrypted = encrypt(text, key);

    std::ofstream outFile(targetFile, std::ios::binary);
    if (!outFile)
    {
      std::cerr << "Failed to open target file.\n";
      return 1;
    }
    // Write checksum as the first byte
    outFile.put(checksum);
    outFile.write(encrypted.c_str(), encrypted.size());
    outFile.close();

    std::cout << "File encrypted successfully.\n";
  }
  else if (mode == "-d")
  {
    if (argc < 3)
    {
      std::cerr << "Missing arguments for decryption.\n";
      return 1;
    }
    std::string encryptedFile = argv[2];

    std::ifstream inFile(encryptedFile, std::ios::binary);
    if (!inFile)
    {
      std::cerr << "Failed to open encrypted file.\n";
      return 1;
    }
    // Read checksum
    char file_checksum = inFile.get();
    std::string encrypted((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
    inFile.close();

    std::string key;
    std::cout << "Enter decryption key: ";
    std::getline(std::cin, key);

    char input_checksum = key_checksum(key);
    if (input_checksum != file_checksum)
    {
      std::cerr << "Incorrect key! Decryption failed.\n";
      // Optionally, output garbage:
      // std::string garbage(encrypted.size(), '*');
      // std::cout << garbage << std::endl;
      return 1;
    }

    std::string decrypted = encrypt(encrypted, key);

    std::cout << "Decrypted content:\n";
    std::cout << decrypted << std::endl;
  }
  else
  {
    std::cerr << "Unknown mode: " << mode << "\n";
    return 1;
  }

  return 0;
}