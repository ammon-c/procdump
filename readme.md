## README - ProcDump utility

ProcDump is a command line utility that prints a list of
currently running processes.  For each process running, the
name, process ID, and several other pieces of information about
the process are printed.  

**Langauge**:  ProcDump is written in C.

**Platform**:  ProcDump compiles for Windows 10 or 11, using the
C compiler that comes with Microsoft Visual Studio 2022.

**Build**:  In a Windows command prompt, **CD** to the directory
containing the ProcDump source code, and type "NMAKE" at the
command prompt.  If successful, the executable program is
written to **procdump.exe** in the same directory.

**Run**:  Run **ProcDump.exe** with no command line parameters
to dump a list of all processes.  Or, run **ProcDump.exe** with
one or more name substrings on the command line to print only
processes whose module name contains one of the substrings.  

---

Example excerpt of program output:
```
...

180: "MoUsoCoreWorker.exe"  pid:19000  parent:18940  children:0  threads:5
181: "ApplicationFrameHost.exe"  pid:15512  parent:1668  children:0  threads:3
          filename: "\Device\HarddiskVolume9\Windows\System32\ApplicationFrameHost.exe"
          command: "C:\Windows\system32\ApplicationFrameHost.exe -Embedding"
182: "svchost.exe"  pid:16604  parent:1520  children:0  threads:1
183: "svchost.exe"  pid:11376  parent:1520  children:0  threads:2
          filename: "\Device\HarddiskVolume9\Windows\System32\svchost.exe"
          command: "C:\Windows\system32\svchost.exe -k DevicesFlow -s DevicesFlowUserSvc"
184: "svchost.exe"  pid:19096  parent:1520  children:0  threads:11
185: "ShellExperienceHost.exe"  pid:18796  parent:1668  children:0  threads:21
          filename: "\Device\HarddiskVolume9\Windows\SystemApps\ShellExperienceHost_cw5n1h2txyewy\ShellExperienceHost.exe"
          command: ""C:\Windows\SystemApps\ShellExperienceHost_cw5n1h2txyewy\ShellExperienceHost.exe" -ServerName:App.AppXtk81tbbce2qsex0s8tw7hfa9xb3t.mca"
186: "RuntimeBroker.exe"  pid:2060  parent:1668  children:0  threads:2
          filename: "\Device\HarddiskVolume9\Windows\System32\RuntimeBroker.exe"
          command: "C:\Windows\System32\RuntimeBroker.exe -Embedding"
187: "svchost.exe"  pid:9696  parent:1520  children:0  threads:5
          filename: "\Device\HarddiskVolume9\Windows\System32\svchost.exe"
          command: "C:\Windows\system32\svchost.exe -k LocalService -p -s NPSMSvc"
188: "AdobeCollabSync.exe"  pid:10376  parent:14244  children:1  threads:7
          filename: "\Device\HarddiskVolume9\Program Files\Adobe\Acrobat DC\Acrobat\AdobeCollabSync.exe"
          command: ""C:\Program Files\Adobe\Acrobat DC\Acrobat\AdobeCollabSync.exe""
189: "AdobeCollabSync.exe"  pid:10824  parent:10376  children:0  threads:19
          filename: "\Device\HarddiskVolume9\Program Files\Adobe\Acrobat DC\Acrobat\AdobeCollabSync.exe"
          command: ""C:\Program Files\Adobe\Acrobat DC\Acrobat\AdobeCollabSync.exe" --type=collab-renderer --proc=10376"
190: "svchost.exe"  pid:14264  parent:1520  children:0  threads:5
191: "GameInputSvc.exe"  pid:17452  parent:1520  children:1  threads:3
192: "GameInputSvc.exe"  pid:18624  parent:17452  children:0  threads:7
193: "backgroundTaskHost.exe"  pid:9576  parent:1668  children:0  threads:11
          filename: "\Device\HarddiskVolume9\Windows\System32\backgroundTaskHost.exe"
          command: ""C:\Windows\system32\backgroundTaskHost.exe" -ServerName:App.AppXfd8mjksncwf4vymkvtzn1jcbs.mca"
...
```
