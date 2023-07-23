# File-Searcher
This program can find any file in your PC.
# Quick install
```
git clone https://github.com/SerhiiPavlyk/Translator
cd File Searcher
git status
md build
cd build
cmake ..
```
# Help
Second parameter = *name of the file to be searched*. <br />
Third parameter(optional) = *search path*. If you do not set the third parameter, the search will be performed by the default path. (C:\\) <br />
If first parameter is *-f* : The results will be output to a file. **"\\out\\output.txt** <br />
If first parameter is	*-s* :The results will be output to the screen. <br />
# Example
```
FindFile.exe -f "README.md" "C:\Users\"
```
