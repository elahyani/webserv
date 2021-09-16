#ifndef CGI_HPP
# define CGI_HPP

# include <string>

struct MetaVariables
{
    std::string contentType; //The data type of the content, used when the client is sending attached content to the server. For example file upload etc.
    std::string contentLength; // The length of the query information that is available only for POST requests.
    std::string httpCookie; // Returns the set cookies in the form of key & value pair.
    std::string httpUserAgent; // The User-Agent request-header field contains information about the user agent originating the request. It is a name of the web browser
    std::string pathInfo; // The path for the CGI script
    std::string queryString; // The URL-encoded information that is sent with GET method request.
    std::string remoteAddr; // The IP address of the remote host making the request. This can be useful for logging or for authentication purpose.
    std::string remoteHost;  // 
    std::string requestMethod; // Method used to request
    std::string scriptFileName; // Full path to script file
    std::string scriptName; // Name of script file
    std::string serverName;
    std::string serverSoftware;
};

class Cgi
{
private:
	struct MetaVariables _envCgi;

public:
	Cgi();
	Cgi(Cgi const &);
	~Cgi();
	Cgi & operator=(Cgi const &);
};

#endif