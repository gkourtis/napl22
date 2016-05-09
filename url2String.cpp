#include <curl/curl.h> 

size_t handle_data(void *ptr, size_t size, size_t nmemb,String* sP){ 
	int numbytes = size*nmemb; 
	return size*nmemb; 
} 

String urlToString(String url){
	String r{};
	CURL* curl = curl_easy_init(); 
	if(!curl) return 0;

	char* s=url.mbs();
		curl_easy_setopt(curl,CURLOPT_URL,s);
	delete[]s;
		curl_easy_setopt(curl,CURLOPT_WRITEDATA,&r);
		curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,handle_data); 
	
	CURLcode res = curl_easy_perform(curl); 
	curl_easy_cleanup(curl); 
	if (res!=0) return 0;
	return r;
}
