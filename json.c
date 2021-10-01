#include <json/json.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>


struct curl_fetch_st{
	char *payload;
	size_t size;
	};





size_t curl_callback (void *contents, size_t size, size_t nmemb, void *userp){
	
		size_t realsize = size * nmemb; // calc buffer size
		struct curl_fetch_st *p = (struct curl_fetch_st *) userp; // cast pointer to fetch struct
		
		// expand buffer
		
		p -> payload = (char *) realloc (p -> payload, p -> size + realsize + 1);
		
		// check buffer
		if(p -> payload == NULL) {
			
			//error handler
			fprintf(stderr, "ERROR: Failed to expand buffer in curl_callback");
			//free buffer
			free(p -> payload);
			return -1;
			}
		//copy content to buffer
		memcpy(&(p -> payload[p -> size]), contents, realsize);
		
		// set new buffer size 
		p->size += realsize;
		
		//ensure null termination
		p->payload[p->size] = 0;
		
		//return size
		return realsize;
	}
	
	
// fetch and return url body via curl
CURLcode curl_fetch_url(CURL *ch,  char *ip_url, struct curl_fetch_st *fetch){
	
	CURLcode rcode;
	
	//init peayload
	fetch -> payload = (char *) calloc(1, sizeof(fetch -> payload));
	
	//check payload
	if(fetch -> payload == NULL){
		//log error
		fprintf(stderr, "ERROR: Failed to allocate payload in curl_getch_url");
		//return error
		return -1;
		}
	//init size
	fetch->size = 0;
	
	
	
	
	
	//set url to fetch
	curl_easy_setopt(ch, CURLOPT_URL, ip_url);
	
	//set callbeck function
	curl_easy_setopt(ch, CURLOPT_WRITEFUNCTION, curl_callback);
	
	//pass fetch struct pointer
	curl_easy_setopt(ch, CURLOPT_WRITEDATA, (void *) fetch);
	
	//set default user agent
	curl_easy_setopt(ch, CURLOPT_USERAGENT, "libcurl-agent/1.0");
	
	//set timeout
	curl_easy_setopt(ch, CURLOPT_TIMEOUT, 150);
	
	//enable location redirects
	curl_easy_setopt(ch, CURLOPT_FOLLOWLOCATION, 1);
	
	//set maximum allowed redirects
	curl_easy_setopt(ch, CURLOPT_MAXREDIRS, 1);
	
	//fetch url
	rcode = curl_easy_perform(ch);

	return rcode;
	}	
	
	int send_json(char* imgBase64, char* ip_url, int cfd){
		
		
		
		
		CURL *ch;
		CURLcode rcode;
		
		json_object *json;
		enum json_tokener_error jerr = json_tokener_success;
		
		
		struct curl_fetch_st curl_fetch;
		struct curl_fetch_st *cf = &curl_fetch;
		struct curl_slist *headers = NULL;
		
		//url to test site
		
		
		//init curl handle
		
		if((ch = curl_easy_init()) == NULL){
			//log error
			fprintf(stderr, "ERROR: failed to create curl hendle in fetch session");
			//return error
			return -1;
			}
		
		//set content type
		headers = curl_slist_append(headers, "Accept: application/json");
		headers = curl_slist_append(headers, "Content-Type: application/json");
		
		
		//creating a json object
		json = json_object_new_object();
		
		//building our onject
		json_object_object_add(json,"title",json_object_new_string("responce"));
		json_object_object_add(json,"body",json_object_new_string(imgBase64));
		
		
		/////////////////////
		
		send(cfd,json_object_to_json_string(json),strlen(json_object_to_json_string(json)),0);
		return 0;
		
		/////////////////////////
		
		
		
		//set curl optins
		curl_easy_setopt(ch, CURLOPT_CUSTOMREQUEST, "POST");
		curl_easy_setopt(ch, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(ch, CURLOPT_POSTFIELDS, json_object_to_json_string(json));
		
		//fetch page and capture return code
		rcode = curl_fetch_url(ch, ip_url ,cf);	
		
		//cleanup curl handle
		curl_easy_cleanup(ch);
		
		//free headers
		curl_slist_free_all(headers);
		//free json object
		json_object_put(json);
		
		//check return code
		if(rcode != CURLE_OK || cf -> size < 1){
			//lof error
			fprintf(stderr, "ERROR: Failed to fetch url (%s) - curl said: (%s)",
					ip_url, curl_easy_strerror(rcode));
			
		return -1;	
		}
		
		//check payload
		
		if(cf -> payload != NULL){
			//print result
			printf("CURL RETURNED: \n%s\n", cf->payload);
			//parse return
			json = json_tokener_parse_verbose(cf->payload, &jerr);
			//free payload
			free(cf -> payload);
			}else{
				//error
				fprintf (stderr, "ERROR: Failed to populate payload");
				//free payload
				free(cf -> payload);
				return -1;
				}
		//check error
		if(jerr != json_tokener_success){
			//error
			fprintf(stderr, "ERROR: Failed to parse json string");
			//free json object
			json_object_put(json);
			return -1;
			}	
		
		
		//free json
		json_object_put(json);	
			
		return 0;
	}
