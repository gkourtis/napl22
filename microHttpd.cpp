obj page1=NAPL(<html><head><meta http-equiv="refresh" content="1"></head><body>);
obj page2=NAPL(</body></html>);
obj pagepath=NAPL(/stack);

static int answer_to_connection(
	void *cls, struct MHD_Connection *connection, 
  const char *url, 
  const char *method, const char *version, 
	const char *upload_data,
  size_t *upload_data_size, void **con_cls
)

{ int r;tString u=newString(url);u.lower();
	if(compare(u,pagepath)==0){
		tString s=concatenateStrings(page1,toString(stack,tsShortForRef),page2);
		char* p=s.mbs();
			MHD_Response * response = MHD_create_response_from_buffer (strlen(p),(void*)p,MHD_RESPMEM_MUST_COPY);
		delete[] p;
		r=MHD_queue_response (connection, MHD_HTTP_OK, response);
		MHD_destroy_response (response);
	}
  return r;
}

