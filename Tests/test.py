import os;
import re;
import subprocess;
import glob;

pattern = re.compile(r'(?P<name>\w*)\.pass\.cpp')
compiler = 'clang.exe'

libraryDir = '..\\Source\\Library'
platform = 'Win32'
platformToOptions = {"Win32" : "m32","x64" : "m64" }
staticLibDir = libraryDir + '\\Lib\\' + platform
compileOptions = '-c -O0 -D "_DEBUG" -D "_DLL" -D "_MT" -frtti -g2 -gdwarf-2 ' \
                 '-fexceptions -fno-ms-compatibility -Xclang --dependent-lib=msvcrtd ' \
                 '-std=c++1z -'+ platformToOptions[platform]  + ' -I ' + libraryDir + ' -I ' + '.\\Support'
binaries = []

for dirName, subdirList, fileList in os.walk('.'):
    for fname in fileList:
        #print(fname)
        res = pattern.match(fname)
        if res is not None:
            binFileName = res.group('name')
            os.system('{} {} {} {}'.format(compiler
                                              , compileOptions , '-o ' + '"' + binFileName + '.o"'
                                              , os.sep.join([dirName, fname])))
            os.system('link {} {}'.format(staticLibDir + '//YupeiLibrary.lib', binFileName + '.o'))
            if glob.glob(binFileName + '.exe'):
                print(os.sep.join([dirName, fname]) + " compiled & linked successfully!\n")
                binaries.append(binFileName + '.exe')
            else:
                print(os.sep.join([dirName, fname]) + " compiled & linked failed!\n")
            print(fname)

for t in binaries:
    subprocess.call(t)


