# Performance benchmarking for CSV-files

## General

File [dfgTestCsvPerformance.cpp](https://github.com/tc3t/dfglib/blob/master/dfgTest/dfgTestCsvPerformance.cpp) implements some CSV-reading benchmarking and results obtained from the code are listed in files [csvPerformanceRuns_sorted_by_xxx.csv](https://github.com/tc3t/dfglib/blob/master/misc/). This file summaries the results.

**Note: The following presents interepretations from rough performance tests. It's strongly adviced to keep in mind that these are personal interpretations and might contain even big mistakes such as not noticing that optimizer has simply removed some code. Corrections are welcome.**

### Description of columns in the raw results file

* \#: Line ID
* Date: Run date
* Test machine: Machine ID
* Compiler: Compiler used in the build
* Pointer size: in bytes
* Build type: debug/release etc.
* File: File that was read.
* Reader & Processing type: The reader used in the test and it's processing type.
    * DelimitedTextReader: Reading was done using dfg::io::DelimitedTextReader.
        * CharAppenderDefault: Default parsing.
        * CharAppenderNone: Adds no characters to read buffer essentially meaning that the file is not parsed, but the time includes the base level overhead from DelimitedTextReader's read implementation.
    * fast-cpp-csv-parse: Parsing was done using fast-cpp-csv-parser.
        * Parse only: The file was parsed, but nothing was stored.
    * readOne()::Read through: The file was not parsed, but just read through byte-by-byte using io::readOne(). Used for comparing stream performance.
    * TableCsv<CharType,IndexType>::Read&Store: File was parsed and stored using TableCsv. Parsing is done using DelimitedTextReader.
* Stream type: Stream type used in the test. For memory stream, includes also the method used to read bytes from file, e.g. '(memory mapped)' means that the bytes were read through memory mapping.
* time#x: Wall clock run time of x'th run.
* time_avg: Average of run times.
* time_median: Median of run times.
* time_sum: Sum of run times.

## 1. Interpretations of runs done in 2016-04-10.

* Tested compilers: VC2010 - VC2015 (update 1), MinGW 4.8.0.

* Raw results: [csvPerformanceRuns_sorted_by_type.csv](https://github.com/tc3t/dfglib/blob/master/misc/csvPerformanceRuns_sorted_by_type.csv)

### General description
Using a generated file of size (4000000 rows, 7 columns, 112 MB) and constant content "abc" on every cells, tests measure the run time using various stream implementations and processing types ranging from simple read through to creation of complete table structure in memory. In addition to using the parsing facility in dfglib, also one presumably fast external csv-parsing library, [fast-cpp-csv-parser](https://github.com/ben-strasser/fast-cpp-csv-parser/), is included to provide some indication of the speed of dfglib's implementation.

### Observations
* Differences between implementations of standard streams in different compilers are big, up to around 7x.
* In all 13 general processing benchmarking types, MinGW 4.8.0 is fastest.
* MinGW is not only fastest in every type, but in many cases the difference is significant: for example in std::istrstream case the fastest VC-implementation is over 3x slower than MinGW.
* VC2010 is slowest in most cases.
* Abstraction penalty introduced by standard streams is significant especially in VC-implementations: for example simply reading through a file one by one in VC2010 takes around 0.2 s with BasicImStream, but the same with std::istrstream takes around 4.8 s, difference is around 23x. As a another example in VC2015 update 1 32-bit it was faster to parse and store the file contents to TableCsv than to simply read it through one by one using std::ifstream.
* fast-cpp-csv-parser was available only on VC2015 and there the fastest dfglib-parsing (test line 78, MSVC_2015_u1 64-bit) was over 4 times slower than fast-cpp-csv-parser.
* Reading the 4e6 * 7 CSV-file to TableCsv took around 4-6 seconds depending on compiler. As a comparison, reading the same file on LibreOffice 5.1.2.2 ended after 25 seconds with note that the data couldn't be completely loaded because the maximum number of rows (=1048576) was exceeded. From this it can be concluded that TableCsv not only can read big files, but it also potentially does it much faster than generic spreadsheet tool such as LibreOffice.

## Miscellaneous

* Details on test machines (listed in raw results file in column 'Test machine')
    * 1: 
        * OS: TODO
        * CPU: TODO
