/*
* GET / HTTP/1.1
* Host: 192.168.0.104:28010
* Connection: keep-alive
* Cache-Control: max-age=0
* Upgrade-Insecure-Requests: 1
* User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_12_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/102.0.0.0 Safari/537.36
* Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*//*;q=0.8,application/signed-exchange;v=b3;q=0.9
* Accept-Encoding: gzip, deflate
* Accept-Language: zh-CN,zh;q=0.9,en;q=0.8
* 
* 
*/
enum Method {
    GET,
    POST
};

struct Resquest {
    enum Method method;   
    char *path;
    char *version;
    char **heads;
    char *body;
};

/*
* HTTP/1.0 200 OK
* Server: SimpleHTTP/0.6 Python/3.8.5
* Date: Tue, 17 May 2022 01:01:39 GMT
* Content-type: text/html; charset=utf-8
* Content-Length: 4589
* 
* <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
* <html>
* <head>
* <meta http-equiv="Content-Type" content="text/html; charset=utf-8">
* <title>Directory listing for /</title>
* </head>
* <body>
* <h1>Directory listing for /</h1>
* <hr>
* <ul>
* <li><a href=".android/">.android/</a></li>
* </ul>
* <hr>
* </body>
* </html>
*/
struct Response {
    char *version;    
    int code;
    char *status;
    char **heads;
    char *body;
};

// str to struct request
void to_request(char *message, struct Request *req);

// struct response to str
void to_response_text(struct Response *resp, char *str);