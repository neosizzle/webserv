#ifndef __REPONSE__H__
#define __REPONSE__H__
#include "webserv.hpp"
#include "Request.hpp"
#include "HttpConfig.hpp"
#include "Logger.hpp"

//class for a single response instance, contains http response data
class Response
{
private:
	std::string							_raw;		//raw http response
	int									_status; 	//http response status
	Request								_req;		//bound request
	std::string							_body;		//response body
	std::map<std::string, std::string>	_headers;	//response headers
	HttpConfig							_config	;	//config for processing request and generating response 
	Logger								_logger;
	//config variables go here.....

	void								_process_get(Request request); //config as param
	void								_process_post(Request request); //config as param
	void								_process_put(Request request); //config as param
	void								_process_delete(Request request); //config as param

	//general utils
	void								_generate_response(int code, std::string body);//generates respons str
	std::string							_resolve_status(int code);//resolves status text given the status code
	int									_parse_form_data(std::string body, std::string boudary, std::map<std::string, std::string> &form_data);//parses form data and returns a map of key value pairs
	void								_generate_err_response(int code, std::string root_path);

	//get utils
	int									_resolve_filepath(std::string route, std::string root, std::string &res);//generates a file path to read from using request data and writes it to filepath
	void								_generate_autoidx(std::string path, std::string root);//generate autoindex

	//post utils
	int									_get_file_upload_name(std::map<std::string, std::string> form_data, std::string &file_name);//returns filename from form data
	int									_get_file_upload_body(std::map<std::string, std::string> form_data, std::string &file_content);//returns file body from form data
	int									_do_upload(std::string filepath, std::string filecontents, int overwrite);//uploads file given the filepath and the filename

	//delete utils
	int									_do_delete(std::string filepath);

public:
	//dummy values
	// std::string	index_file = "index.html";
	// std::string	not_found_file = "error_pages/404.html";

	Response();
	~Response();
	Response(const Response &other);
	Response &operator=(const Response &rhs);

	//getters
	std::string							get_response();
	int									get_status();
	Request								get_request();
	std::string							get_body();
	std::map<std::string, std::string>	get_headers();

	void								call(Request request, HttpConfig requestConfig); //config as param

};
#endif  //!__REPONSE__H__