int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	WSADATA wsadata;
	if ( WSAStartup( MAKEWORD( 2, 2 ), &wsadata ) != 0 )
	{
		return 0;
	}
	HANDLE hMutex = CreateMutex( NULL, FALSE, "ASYNC_SERVER" );
	if ( GetLastError() != ERROR_ALREADY_EXISTS )
	{
		DialogBox( hInstance, MAKEINTRESOURCE( IDD_DIALOG1 ), 0, DlgProc );
		CloseHandle( hMutex );
		WSACleanup();
		return 0;
	}
	MessageBox( NULL, "이미 실행 중 입니다", "경고", MB_OK );
	CloseHandle( hMutex );
	WSACleanup();
	return 0;
}
BOOL CALLBACK DlgProc( HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam )
{
	switch ( iMessage )
	{
	case WM_INITDIALOG:  return InitSock( hDlg );
	case WM_COMMAND:  return DlgCmd( hDlg, wParam );
	case MWM_SOCK:  return Onsock( hDlg, (SOCKET)wParam, LOWORD( lParam ), HIWORD( lParam ) );
	}
	return FALSE;
}
BOOL DlgCmd( HWND hDlg, WPARAM wParam )
{
	switch ( LOWORD( wParam ) )
	{
	case IDCANCEL: EndDialog( hDlg, IDD_DIALOG1 );
	}
	return TRUE;
}
BOOL InitSock( HWND hDlg )
{
	SOCKET sock = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );
	SOCKADDR_IN servaddr = { 0, };
	servaddr.sin_addr.s_addr = inet_addr( MY_IP );
	servaddr.sin_port = htons( USE_PORT );
	servaddr.sin_family = PF_INET;

	bind( sock, (SOCKADDR *)&servaddr, sizeof(servaddr) );
	listen( sock, 3 );
	WSAAsyncSelect( sock, hDlg, MWM_SOCK, FD_ACCEPT );
	return TRUE;
}
BOOL Onsock( HWND hDlg, SOCKET sock, WORD msg, WORD eid )
{
	switch ( msg )
	{
	case FD_ACCEPT: AcceptProc( hDlg, sock ); break;
	case FD_READ: ReadMSG( hDlg, sock );  break;
	}
	return TRUE;
}
void AcceptProc( HWND hDlg, SOCKET sock )
{
	SOCKADDR_IN clientaddr = { 0, };
	int len = sizeof(clientaddr);
	SOCKET dosock = accept( sock, (SOCKADDR *)&clientaddr, &len );
	WSAAsyncSelect( dosock, hDlg, MWM_SOCK, FD_READ );
}
void ReadMSG( HWND hDlg, SOCKET sock )
{
	int MSGlen;
	char msg[256];
	memset( msg, 0, 256 );
	recv( sock, (char*)&MSGlen, sizeof(MSGlen), 0 );
	recv( sock, msg, MSGlen, 0 );
	//MessageBox(hDlg,"메세지가 도착하였습니다","OK",MB_OK);
	SendMessage( GetDlgItem( hDlg, IDC_LIST1 ), LB_ADDSTRING, 0, (LPARAM)msg );//list에 추가!
}