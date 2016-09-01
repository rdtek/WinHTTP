# WinHTTP
Windows cmd line tool to send HTTP requests  

### Usage

*Simple GET request*  
Response printed to console.
```
C:\winhttp http://www.google.com
```  

GET request, response saved to file using `--output` (`-o`) flag. 
```
C:\winhttp http://www.google.com -o response.html
```  

HEAD request if server supports it, this fetches headers only, no body.  
The `--method` (`-m`) flag allows the HTTP method to be specified, default is GET. 
```
C:\winhttp http://www.google.com -m HEAD
```  
Verbose mode using `--verbose` (`-v`) flag.
```
C:\winhttp http://www.google.com -v
```  

![Command line screenshot](winhttp_cmd.png?raw=true)
