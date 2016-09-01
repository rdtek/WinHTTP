# WinHTTP
Windows cmd line tool to send HTTP requests  

### Usage Examples

**1. Simple GET request**  
Response printed to console.
```
C:\winhttp http://www.google.com
```  
**2. Save response**  `--output` (`-o`)    
GET request, response saved to file using `--output` (`-o`) flag. 
```
C:\winhttp http://www.google.com -o response.html
```  
**3. Request headers only**  
HEAD request if server supports it, this fetches headers only, no body.  
The `--method` (`-m`) flag allows the HTTP method to be specified, default is GET. 
```
C:\winhttp http://www.google.com -m HEAD
```  
**4. Verbose**  
Verbose mode using `--verbose` (`-v`) flag.
```
C:\winhttp http://www.google.com -v
```  

![Command line screenshot](winhttp_cmd.png?raw=true)
