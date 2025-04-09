# Higher Order functions using C++(using std::function)

## What are higher order functions
A higher order function is a function which takes another function(s) as a parameter and/or returns another functions. They are far more common that we might think.
In fact, we have already spent a lot of time with higher order 	functions in school, some of the examples are:

| Function                                    | Inputs function(s)    | Returns function(s) |
|---------------------------------------------|-----------------------|---------------------|
| Derivative:<br>f'(x->x^2) = x->(2*x)        | Y                     | Y                   |
| Definite Integral:<br>1,2∫ (x->x) = 3/2     | Y                     | N                   |
| Indefinite integral:<br>∫(x->x) = x->(x^2)/2| Y                     | Y                   |
| Inversion:<br>f-1(x->x^2) = x->√x           | Y                     | Y                   |
| Composition:<br>(f∘g)(x)=f(g(x))            | Y                     | Y                   |

## Examples of higher order functions in c++
### std::sort:
```C++
std::vector<uint32_t> vec{1,2,3,........,1000000}
//Sort ascending
std::sort(vec.begin(), vec.end(), [](const uint32_t n1, const uint32_t n2){ return n1 < n2; })

//Sort descending
std::sort(vec.begin(), vec.end(), [](const uint32_t n1, const uint32_t n2){ return n1 > n2; })
```
### std::accumulate:
```C++
std::vector<uint32_t> vec{1,2,3,........,1000000}

//Calculate sum of vector elements
auto sum = std::accumulate(vec.begin(), vec.end(), 0, [](const uint32_t n1, const uint32_t n2){ return n1 + n2; })

//Calculate sum of squares vector elements
auto sumSqr = std::accumulate(vec.begin(), vec.end(), 0, [](const uint32_t n1, const uint32_t n2){ return n1 + n2*n2; })
```
## More examples(using std::function feature):
```C++
  double sqr(double n){
    return n*n;
  }
  //Using lambdas
  std::function<double(double)> sqrFunc = [](double n){
    return n*n;
  };

  std::cout << sqr(2);//Prints 4

  //Using function pointer assignment
  sqrFunc = sqr;
  std::cout << sqr(3);//Prints 9
```
  std::function can also be used to provide a common interface so that the
  external code can pass plain function or lambda or any other equivalent thing, example:

```C++
  typedef std::vector<double> RealNumVector;
  //A function that takes a vector of double as input and produces a double as output
  typedef std::function<double(const RealNumVector&)> VectorOperator;
  double getMiddleNumber(const RealNumVector& arr, const VectorOperator& minFetcher, const VectorOperator& maxFetcher)
  {
    return (minFetcher(arr) + maxFetcher(arr)) / 2;
  }

  
  std::vector<double> arr{2.0, 1.0, 4.0, 3.0, 5.0};

  double getMinElem(const RealNumVector& arr) {
    double min = 0xffffffffffffffff;
    for(auto const num : arr) {
      if(num < min){
        min = num;
      }
    }
    return min;
  }

  double getMaxElem(const RealNumVector& arr) {
    double max = 0.0;
    for(auto const num : arr) {
      if(num > max){
        max = num;
      }
    }
    return max;
  }
  //Passing function pointer 
  std::cout << getMiddleNumber(arr, getMinElem, getMaxElem);

  //Passing lambdas, very javascripty :)
  std::cout << getMiddleNumber(arr, 
    [](const RealNumVector& arr) {
    double min = 0xffffffffffffffff;
    for(auto const num : arr) {
      if(num < min){
        min = num;
      }
    }
    return min;
  },
  [](const RealNumVector& arr) {
    double max = 0.0;
    for(auto const num : arr) {
      if(num > max){
        max = num;
      }
      return max;
    }
  });
```

## Use cases for Higher Order Functions:
### Lazy expression evaluation:
Lazy evaluation is essentially way to program, strictly adhering to the idea that an expression
should only be evaluated when the program can’t function without the result it yields.

Any calculation or IO should be done only because its result is absolutely required,  not because it may be required.
Higher orders functions are a tool that enable us to achieve this.

Let me try to elaborate with a few examples:
Consider the following code:

```C++  

double getMean(const std::vector<double>& vec);

double getMedian(const std::vector<double>& vec);

double getStdDev(const std::vector<double>& vec);

// Returns the middle range of a dataSet, for example, if the dataset is the yearly incomes of
// a population, then the result may represent the income range of the middle class 
std::tuple<double, double> getMiddleRange(double mean, double middle, double stdDev)
{
  //Mean and median are not too far, use the formula below
  if (fabs((mean - middle)/mean) < 0.2)
  {
    return {0.5*mean, 1.5* mean};
  }
  //Mean and median are way too far, so the data is skewed, use the stdDev formula
  else
  {
    return { mean - 2*stdDev, mean + 2*stdDev};
  }
}

  
int main()
{
  std::vector<double> arr;
  for(uint32_t i = 0; i < 1000000; i++)
  {
      arr.push_back(rand());
  }

  auto const mean = getMean(arr);
  auto const median = getMedian(arr);
  auto const stdDev = getStdDev(arr);

  auto const& [start, end] = getMiddleRange(mean, median, stdDev);
  std::cout << "Middle range is: " << start << "-" << end;
}

```

There is a big problem with this code, in the case when the function does not need stdDev to calculate
the result, the stdDev is calculated regardless as hving stdDev is a pre-requisite to call 'getMiddleRange'
function. This means wasting a lot of computation in some cases.

#### Higher order function approach:
```C++
typedef std::function<double(const std::vector<double>&)> Datafetcher;

double getMean(const std::vector<double>& vec);

double getMedian(const std::vector<double>& vec);

double getMinMaxAvg(const std::vector<double>& vec);//Returns (min + max)/2

double getStdDev(const std::vector<double>& vec);

// Returns the middle range of a dataSet, for example, if the dataset is the yearly incomes of
// a population, then the result may represent the income range of the middle class 
std::tuple<double, double> getMiddleRange(const std:vector<double>& vec,
                                          const DataFetcher& meanFetcher,
                                          const DataFetcher& middleElementFetcher,
                                          const DataFetcher& stdDevFetcher)
{
  auto mean = meanFetcher();
  auto middle = middleElementFetcher(vec);
  //Mean and median are not too far, use the formula below
  if (fabs((mean - median)/mean) < 0.2)
  {
    return {0.5*mean, 1.5* mean};
  }
  //Mean and median are way too far, so the data is skewed, use the stdDev formula
  else
  {
    auto stdDev = stdDevFetcher();
    return { mean - 2*stdDev, mean + 2*stdDev};
  }
}

int main()
{
    std::vector<double> vec;
    for(uint32_t i = 0; i < 1000000; i++)
    {
        vec.push_back(rand());
    }

    //Middle element as median
    double start, end;
    std::tie(start, end) = getMiddleRange(vec,
                                          [&arr]{ return getMean(arr); }, 
                                          [&arr]{ return getMedian(arr); },
                                          [&arr]{ return getStdDev(arr); });
    std::cout << "Middle range is: " << start << "-" << end;

    //Middle element as median (min + max)/2
    std::tie(start, end) = getMiddleRange([&arr]{ return getMean(arr); }, 
                                          [&arr]{ return getMinMaxAvg(arr); },
                                          [&arr]{ return getStdDev(arr); });
}
```

In this version, stdDev is calculated only when the it required
Apart from that, we can easily plugin the logic(s) we want into the
'getMiddleRange' method, which was not possible in the previous example.

### Logic Substitution:

Going back to previous example, one could argue that higher order functions were an
unworthy piece of complexity added to our design as this issue could be addressed by
passing the vector directly to 'getMiddleRange' function
and selectively call the 'getStdDev' there, only when needed, so the code would seem like:
```C++
double getMean(const std::vector<double>& vec);

double getMedian(const std::vector<double>& vec);

double getStdDev(const std::vector<double>& vec);

// Returns the middle range of a dataSet, for example, if the dataset is the yearly incomes of
// a population, then the result may represent the income range of the middle class 
std::tuple<double, double> getMiddleRange(const std::vector<double>& vec)
{
  auto mean = getMean(vec);
  auto middle = getMedian(vec);

  //Mean and median are not too far, use the formula below
  if (fabs((mean - median)/mean) < 0.2)
  {
    return {0.5*mean, 1.5* mean};
  }
  //Mean and median are way too far, so the data is skewed, use the stdDev formula
  else
  {
    auto stdDev = getStdDev(vec);
    return { mean - 2*stdDev, mean + 2*stdDev};
  }
}

int main()
{
    std::vector<double> arr;
    for(uint32_t i = 0; i < 1000000; i++)
    {
        arr.push_back(rand());
    }

    auto const& [start, end] = getMiddleRange(arr);
    std::cout << "Middle range is: " << start << "-" << end;
}
```

This may seem harmless at 1st glance but this approach falls short as our
requirements grow in complexity. The reusablility of this function has decreased.
For instance, what happens when we want the 'middle' element to be the average
of min and max in some cases, that case, we will have to write a new function.

Above mentioned issues are addressed if chose 'getMiddleRange'
to be a higher order function. The code below illustrated this:

```C++
typedef std::function<double()> Datafetcher;

double getMean(const std::vector<double>& vec);

double getMedian(const std::vector<double>& vec);

double getMinMaxAvg(const std::vector<double>& vec);//Returns (min + max)/2

double getStdDev(const std::vector<double>& vec);

// Returns the middle range of a dataSet, for example, if the dataset is the yearly incomes of
// a population, then the result may represent the income range of the middle class 
std::tuple<double, double> getMiddleRange(const DataFetcher& meanFetcher,
                                          const DataFetcher& middleElementFetcher,
                                          const DataFetcher& stdDevFetcher)
{
  auto mean = meanFetcher();
  auto middle = middleElementFetcher(vec);
  //Mean and median are not too far, use the formula below
  if (fabs((mean - median)/mean) < 0.2)
  {
    return {0.5*mean, 1.5* mean};
  }
  //Mean and median are way too far, so the data is skewed, use the stdDev formula
  else
  {
    auto stdDev = stdDevFetcher();
    return { mean - 2*stdDev, mean + 2*stdDev};
  }
}

int main()
{
    std::vector<double> arr;
    for(uint32_t i = 0; i < 1000000; i++)
    {
        arr.push_back(rand());
    }

    //Middle element as median
    double start, end;
    std::tie(start, end) = getMiddleRange([&arr](){ return getMean(arr); }, 
                                          [&arr](){ return getMedian(arr); },
                                          [&arr](){ return getStdDev(arr); });
    std::cout << "Middle range is: " << start << "-" << end;

    //Middle element as median (min + max)/2
    std::tie(start, end) = getMiddleRange([&arr](){ return getMean(arr); }, 
                                          [&arr](){ return getMinMaxAvg(arr); },
                                          [&arr](){ return getStdDev(arr); });

    std::cout << "Middle range is: " << start << "-" << end;
}
```

In this version, stdDev is calculated only when the it required
Apart from that, we can easily plugin the logic(s) we want into the
'getMiddleRange' method, which was not possible in the previous example.

      