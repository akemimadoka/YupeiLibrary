import os;
import re;

pattern = re.compile(r'(?P<name>\w*)pass\.cpp')
compiler = 'clang'

libraryDir = '..\\Source\\Library'
staticLibDir = libraryDir + '\\Lib\\x64'
compileOptions = '-O0 -std=c++1z -m64 -I ' + libraryDir #+ ' -L'+ staticLibDir

for dirName, subdirList, fileList in os.walk('.'):
    for fname in fileList:
        #print(fname)
        res = pattern.match(fname)
        if res is not None:
            os.system('{} {} {} {} {}'.format(compiler, '-o ' + res.group('name'), compileOptions, staticLibDir +
                                              "\\YupeiLibrary.lib"
                                              , os.sep.join([dirName, fname])))
            print(fname)

