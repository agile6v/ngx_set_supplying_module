/*
 * Copyright (C) liu_wei
 */

#include <ndk.h>

static char * 
	ngx_http_set_supplying_set_ori_src_ip(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static char * 
	ngx_http_set_supplying_set_dst_src_ip(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static ngx_int_t    
	ngx_http_set_supplying_get_ori_src_ip(ngx_http_request_t *r, ngx_str_t *val, ngx_http_variable_value_t *v);
static ngx_int_t    
	ngx_http_set_supplying_get_dst_src_ip(ngx_http_request_t *r, ngx_str_t *val, ngx_http_variable_value_t *v);

static ngx_command_t ngx_http_set_supplying_commands[] = {

	{ ngx_string("set_ori_src_ip"),
	  NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_SIF_CONF|
			NGX_HTTP_LOC_CONF|NGX_HTTP_LIF_CONF|NGX_CONF_TAKE2,
	  ngx_http_set_supplying_set_ori_src_ip,
	  0,			
	  0,
	  NULL },
	  
	{ ngx_string("set_ori_dst_ip"),
	  NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_SIF_CONF|
			NGX_HTTP_LOC_CONF|NGX_HTTP_LIF_CONF|NGX_CONF_TAKE2,
	  ngx_http_set_supplying_set_dst_src_ip,
	  0,			
	  0,
	  NULL },  

	ngx_null_command
};


static ngx_http_module_t  ngx_http_set_supplying_module_ctx = {
	NULL,								   /* preconfiguration */
	NULL,                   			   /* postconfiguration */

	NULL,                                  /* create main configuration */
	NULL,                                  /* init main configuration */

	NULL,                                  /* create server configuration */
	NULL,                                  /* merge server configuration */

	NULL,    							   /* create location configuration */
	NULL      	   						   /* merge location configuration */
};

ngx_module_t  ngx_http_set_supplying_module = {
	NGX_MODULE_V1,
	&ngx_http_set_supplying_module_ctx,    /* module context */
	ngx_http_set_supplying_commands,           /* module directives */
	NGX_HTTP_MODULE,                       /* module type */
	NULL,                                  /* init master */
	NULL,                                  /* init module */
	NULL,                                  /* init process */
	NULL,                                  /* init thread */
	NULL,                                  /* exit thread */
	NULL,                                  /* exit process */
	NULL,                                  /* exit master */
	NGX_MODULE_V1_PADDING
};

static char * 
ngx_http_set_supplying_set_ori_src_ip(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
	ngx_str_t         *var_name;
    ndk_set_var_t      filter;

    var_name = cf->args->elts;

    filter.type = NDK_SET_VAR_MULTI_VALUE;
    filter.func = ngx_http_set_supplying_get_ori_src_ip;
    filter.size = 2;
	filter.data = NULL;

    return  ndk_set_var_multi_value_core (cf, &var_name[1], &var_name[1], &filter);
}

static char * 
ngx_http_set_supplying_set_dst_src_ip(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
	ngx_str_t         *var_name;
    ndk_set_var_t      filter;

    var_name = cf->args->elts;

    filter.type = NDK_SET_VAR_MULTI_VALUE;
    filter.func = ngx_http_set_supplying_get_dst_src_ip;
    filter.size = 2;
	filter.data = NULL;

    return  ndk_set_var_multi_value_core (cf, &var_name[1], &var_name[1], &filter);
}


static ngx_int_t
ngx_http_set_supplying_get_ori_src_ip(ngx_http_request_t *r, 
											ngx_str_t *val, ngx_http_variable_value_t *v)
{
	ngx_list_part_t              *part;
	ngx_table_elt_t              *header;
	ngx_uint_t					 i;
	ngx_connection_t			 *c;
	ngx_http_variable_value_t  	 *v_hdr, *v_value;
	
	c = r->connection;

	v_hdr = v + 1;
	v_value	= v;
	
	if (c->ori_src_ip) {
	
		val->data = c->ori_src_ip->data;
		val->len = c->ori_src_ip->len;
		
	} else {
	
		if (r == r->main && c->requests == 1) {
		
			//	连接中第一个请求将被检查是否含有指定头
			part = &r->headers_in.headers.part;
			header = part->elts;

			if (v_hdr->len) {
				
				for (i = 0; /* void */; i++) {

					if (i >= part->nelts) {
						if (part->next == NULL) {
							break;
						}

						part = part->next;
						header = part->elts;
						i = 0;
					}

					//	在客户端的请求头域中查找指定header
					if (ngx_strncasecmp(header[i].lowcase_key, v_hdr->data, v_hdr->len) == 0) {	
						
						ndk_palloc_re(c->ori_src_ip, c->pool, sizeof(ngx_str_t));	
						ndk_palloc_re(c->ori_src_ip->data, c->pool, header[i].value.len);
						
						c->ori_src_ip->len = header[i].value.len;

						ngx_memcpy (c->ori_src_ip->data, header[i].value.data, header[i].value.len);
						
						val->data = c->ori_src_ip->data;
						val->len = c->ori_src_ip->len;
						
						return NGX_OK;
					}
				}	//	end for
			}
		} 
		
		ngx_str_null(val);
	}
	
	return NGX_OK;
}

static ngx_int_t
ngx_http_set_supplying_get_dst_src_ip(ngx_http_request_t *r, 
											ngx_str_t *val, ngx_http_variable_value_t *v)
{
	ngx_list_part_t              *part;
	ngx_table_elt_t              *header;
	ngx_uint_t					 i;
	ngx_connection_t			 *c;
	ngx_http_variable_value_t  	 *v_hdr, *v_value;
	
	c = r->connection;

	v_hdr = v + 1;
	v_value	= v;
	
	if (c->ori_dst_ip) {
	
		val->data = c->ori_dst_ip->data;
		val->len = c->ori_dst_ip->len;
		
	} else {
	
		if (r == r->main && c->requests == 1) {
		
			//	连接中第一个请求将被检查是否含有指定头
			part = &r->headers_in.headers.part;
			header = part->elts;

			if (v_hdr->len) {
				
				for (i = 0; /* void */; i++) {

					if (i >= part->nelts) {
						if (part->next == NULL) {
							break;
						}

						part = part->next;
						header = part->elts;
						i = 0;
					}

					//	在客户端的请求头域中查找指定header
					if (ngx_strncasecmp(header[i].lowcase_key, v_hdr->data, v_hdr->len) == 0) {	
						
						ndk_palloc_re(c->ori_dst_ip, c->pool, sizeof(ngx_str_t));	
						ndk_palloc_re(c->ori_dst_ip->data, c->pool, header[i].value.len);
						
						c->ori_dst_ip->len = header[i].value.len;

						ngx_memcpy (c->ori_dst_ip->data, header[i].value.data, header[i].value.len);
						
						val->data = c->ori_dst_ip->data;
						val->len = c->ori_dst_ip->len;
						
						return NGX_OK;
					}
				}	//	end for
			}
		}
		
		ngx_str_null(val);
	}
	
	return NGX_OK;
}


