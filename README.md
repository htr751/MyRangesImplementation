# MyRangesImplementation
a simple implmenetation of some ranges facilities of the std::experimental::ranges::view namespace distributed as a header only library for c++14/17/20.

#About
In general ranges are built as an abstraction of top on regular iterators and they provide two main things:
*they make it able to replace to traditional way of passing containers to algorithms which is by passing a begin and end iterator, by a simpler way of pass the whole container to the algorithm (so instead of calling std::transform(vec.begin(), vec.end(),...) you would just call std::transform(vec,...)).
* they make it possible to apply algorithms to the containers (now referred to as ranges) on views of those containers.                    for example up say we wanted to filter out the vector {1,2,3,4,5,6} for odd numbers and then to transform the resulted vector by multiply each element by 2. if we don't want to change to original container so without ranges and because of the constraints that the current algorihtms of the STL posses, we would end up with making two copies of the container. but if we would use ranges::view we could do all the operations of the filtering and the transforming on a view of the original container and only at the end to copy the final results to vector for example.

#Features of the library
the library currently supports the following featurs:
*transform range which gets an underlying range(a range is anything with begin and end functions that returns iteators) and a function(anyhting with operator () that gets as parameter the underlying type of the underlying range) and make a view range out of it so while going forward with this range iterators, the result of getting a value from the iterator is the value the transform function returns when supplying to it the underlying value that the iterator references from the underlying range.
*filter range which gets a range and a filter function and make a view range out of it so that while going forward with the range iterator the iterator will return only the values from the original range that the filter function returns true on them/
*zip range which gets two ranges and make a viw range which his iterator returns pair of two value, one is the currently referenced value from the first ranges and another one is the currently referenced value from the second range. for example givan ranges {1,2,3} and {4,5,6} the zip range will create a range that contains {{1,4},{2,5},{3,6}}.
*cartezian product range which gets a variadic amount of ranges and make a range that contains the cartezian product of all supplied range. for example when doing cartezian product on {1,2}, {3,4},{5,6} we will get {{1,3,5},{1,3,6},{1,4,5},{1,4,6},{2,3,5},{2,3,6},{2,4,5},{2,4,6}}.
*ints range which returns a range that contains almost all the ints existing starting from a given value.
*take range that takes another range and makes a view range that returns only the first x number of elements from the underlying range.

all those ranges are view ranges and are lazy evaluated(the actual actions happen only when you ask them to happen, for example when transforming the range to a vector).

#Usage
for each of the above ranges there is a function with the same name that makes that range, while supplying underlying ranges(if necessary) using the pipeline operator(operator |).
for example, lets say I want to create a vector which contains only the odd numbers of a the vector {1,2,3,4,5,6} multiplied by 2.
Then I will do the following:
```std::vector<int> vec{1,2,3,4,5,6};
   std::vector<int> result = vec | ranges::view::filter([](int x){return x%2!=0;}) | ranges::view::transform([](int x){return x*2;});
   ```
   and then result will contain {2,6,10}.
#Testing
all the library featurs are heavily tested using catch2 unit testing.
#Usage
you can use the library by including a specific header from the headers distributed in this repo or by includng the ranges header which contains all the library in single header.


