#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void error_handling(char *message);

int main(int argc, char *argv[])
{
	int serv_sd, clnt_sd;//서버, 클라이언트 소켓 정수 선언 
	FILE * fp; //파일 포인터 선언 
	char buf[BUF_SIZE];  //버퍼사이즈 크기의 문자열 선언 
	int read_cnt;  //읽은 개수 선언 
	
	struct sockaddr_in serv_adr, clnt_adr; //소켓구조체 서버, 클라이언트 선언 
	socklen_t clnt_adr_sz; //소켓길이 클라이언트 
	
	if(argc!=2) { //사용방법 : 실행파일이름 포트번호  ex)  ./fserver 9091 
		printf("Usage: %s <port>\n", argv[0]);
		exit(1);
	}
	
	fp=fopen("file_server.c", "rb");  //rb타입으로 file_server.c 파일 열기 
	serv_sd=socket(PF_INET, SOCK_STREAM, 0);  //서버 소켓 선언 
	
	memset(&serv_adr, 0, sizeof(serv_adr));  //서버 주소 정보 메모리할당 
	serv_adr.sin_family=AF_INET; //소켓 타입, 주소, 포트 번호 선언 
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_adr.sin_port=htons(atoi(argv[1]));
	
	bind(serv_sd, (struct sockaddr*)&serv_adr, sizeof(serv_adr)); //소켓 바인드 준비 
	listen(serv_sd, 5); //5개 까지 클라이언트 받을 수 있음 
	
	clnt_adr_sz=sizeof(clnt_adr);  //클라이언트 accept 준비 
	clnt_sd=accept(serv_sd, (struct sockaddr*)&clnt_adr, &clnt_adr_sz); //클라이언트 받아들일 때마다 새로운 소켓 생성 
	
	while(1)
	{
		read_cnt=fread((void*)buf, 1, BUF_SIZE, fp);
		if(read_cnt<BUF_SIZE)
		{
			write(clnt_sd, buf, read_cnt);
			break; //연결된 클라이언트에게 파일 데이터를 30개씩 전달한다??
		}
		write(clnt_sd, buf, BUF_SIZE);
	}
	
	shutdown(clnt_sd, SHUT_WR);	//파일전송 출력 스트림만 닫기, 클라이언트에게 EOF 전달
	read(clnt_sd, buf, BUF_SIZE); //파일전송 입력 스트림은 유지 
	printf("Message from client: %s \n", buf); //클라이언트로부터 데이터 받기 
	
	fclose(fp); 
	close(clnt_sd); close(serv_sd); //파일닫기 -> 클라이언트 닫기 -> 서버 닫기 
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
