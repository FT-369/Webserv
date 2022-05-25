#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <string>

void    cgiSetEnv()
{
    cgi_env["SERVER_PROTOCOL"]
    cgi_env["SERVER_PORT"]
    cgi_env["SERVER_NAME"]
    cgi_env["SERVER_SOFTWARE"]
    cgi_env["REQUEST_METHOD"]
    cgi_env["PATH_INFO"]
    cgi_env["PATH_TRANSLATED"]
    cgi_env["GATEWAY_INTERFACE"]
    cgi_env["SCRIPT_NAME"]
    cgi_env["QUERY_STRING"]
    cgi_env["REMOTE_ADDR"]
    cgi_env["REMOTE_USER"]
    cgi_env["REMOTE_HOST"]
    cgi_env["CONTENT_LENGTH"]
    cgi_env["CONTENT_TYPE"]
    cgi_env["REQUEST_URI"]



SERVER_PROTOCOL = HTTP/version 클라이언트 요청이 사용하는 프로토콜
REQUEST_METHOD = HTTP_method HTML 폼이 사용하는 METHOD
PATH_INFO = extra_path 클라이언트에 의해 전달되는 추가 PATH 정보
PATH_TRANSLATED = trans1/extra_path PATH_INFO의 가상 경로(path)를 물리적인 경로로 변환
SCRIPT_NAME = /path/script_name 실행된 스크립트에 대한 가상 경로
QUERY_STRING = query_string URL에서 "?" 다음에 오는 질의어 문자열
REMOTE_ADDR = ###,###,### 클라이언트의 IP주소
REMOTE_HOST = domain.name 클라이언트의 호스트 이름
REMOTE_USER = name 접근 권한을 얻은 사용자의 검증된 이름
REMOTE_IDENT = name 검증 데몬을 이용해 서버로부터 검증된 원거리 사용자 이름. 이 변수의 용도는 로그인 에만 한정
CONTENT_LENGTH = length POST 방식일 경우 클라이언트에서 보내진 사용자 입력의 길이를 표시
CONTENT_TYPE = MIME_TYPE 클라이언트
}