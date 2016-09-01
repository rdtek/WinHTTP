# WinHTTP
Windows cmd line tool to send HTTP requests  

### Usage Examples

**1. Simple GET request**  
```
C:\winhttp http://www.google.com
```  
**2. Save response**  `--output` (`-o`)  
```
C:\winhttp http://www.google.com -o response.html
```  
**3. Request headers only**  `--method` (`-m`) `HEAD`  
Works if server supports HEAD requests.  
```
C:\winhttp http://www.google.com -m HEAD
```  
**4. Verbose** `--verbose` (`-v`) flag.  
```
C:\winhttp http://www.google.com -v
```  

![Command line screenshot](winhttp_cmd.png?raw=true)
