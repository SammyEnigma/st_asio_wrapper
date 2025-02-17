
#include <iostream>

//configuration
#define ST_ASIO_SERVER_PORT		5051
#define ST_ASIO_RESTORE_OBJECT
#define ST_ASIO_ENHANCED_STABILITY
#define ST_ASIO_WANT_MSG_SEND_NOTIFY
#define ST_ASIO_INPUT_QUEUE non_lock_queue
//file_server / file_client is a responsive system, before file_server send each message (except talking message,
//but file_server only receive talking message, not send talking message proactively), the previous message has been
//sent to file_client, so sending buffer will always be empty, which means we will never operate sending buffer concurrently,
//so need no locks.
#define ST_ASIO_DEFAULT_PACKER	packer2<>
#define ST_ASIO_RECV_BUFFER_TYPE std::vector<boost::asio::mutable_buffer> //scatter-gather buffer, it's very useful under certain situations (for example, ring buffer).
#define ST_ASIO_SCATTERED_RECV_BUFFER //used by unpackers, not belongs to st_asio_wrapper
//configuration

#include "file_socket.h"

#define QUIT_COMMAND	"quit"
#define RESTART_COMMAND	"restart"
#define STATUS			"status"
#define STATISTIC		"statistic"
#define LIST_ALL_CLIENT	"list all client"

int main(int argc, const char* argv[])
{
	puts("this is a file transfer server.");
	printf("usage: %s [<port=%d> [ip=0.0.0.0]]\n", argv[0], ST_ASIO_SERVER_PORT);
	if (argc >= 2 && (0 == strcmp(argv[1], "--help") || 0 == strcmp(argv[1], "-h")))
		return 0;
	else
		puts("type " QUIT_COMMAND " to end.");

	service_pump sp;
	server_base<file_socket> file_server_(sp);

	if (argc > 2)
		file_server_.set_server_addr(atoi(argv[1]), argv[2]);
	else if (argc > 1)
		file_server_.set_server_addr(atoi(argv[1]));

	sp.start_service();
	while(sp.is_running())
	{
		std::string str;
		std::getline(std::cin, str);
		if (str.empty())
			;
		else if (QUIT_COMMAND == str)
			sp.stop_service();
		else if (RESTART_COMMAND == str)
		{
			sp.stop_service();
			sp.start_service();
		}
		else if (STATISTIC == str)
		{
			printf("link #: " ST_ASIO_SF ", invalid links: " ST_ASIO_SF "\n\n", file_server_.size(), file_server_.invalid_object_size());
			puts(file_server_.get_statistic().to_string().data());
		}
		else if (STATUS == str)
			file_server_.list_all_status();
		else if (LIST_ALL_CLIENT == str)
			file_server_.list_all_object();
	}

	return 0;
}
