#include "cl_local.h"

// Write the host's address to a buffer
// NOTE: On browser, it simply connects to /socket. A dedicated PC version is
// not supported yet so it simply connects to localhost for testing purposes.
#ifdef __EMSCRIPTEN__
void cl_get_host_address(char *host_address, int len);
#else
void cl_get_host_address(char *host_address, int len)
{
  const char *str_host = "127.0.0.1";
  memcpy(host_address, str_host, strlen(str_host) + 1);
}
#endif

void cl_connect()
{
  // Connect to server
  char host_address[256];
  cl_get_host_address(host_address, 256);
  
  log_printf(LOG_DEBUG, "connecting...");
  
  cl.connection = CONN_CONNECTING;
  cl.sock = net_connect(host_address);
}

void cl_parse()
{
  if (cl.connection == CONN_DISCONNECTED)
    return;
  
  // TODO: Add client disconnect checks; where frame_read() == 0
  frame_t frame;
  while (frame_read(cl.sock, &frame) > 0) {
    switch (frame.netcmd) {
    case NETCMD_CLIENT_INFO:
      cl_parse_client_info(&frame);
      break;
    case NETCMD_SNAPSHOT:
      cl_parse_snapshot(&frame);
      break;
    case NETCMD_CHAT:
      cl_parse_chat(&frame);
      break;
    // The client should not receive these packets from the server as they are
    // sent by the client
    case NETCMD_SCORE:
    case NETCMD_USERCMD:
    case NETCMD_NAME:
      log_printf(LOG_WARNING, "cl_parse(): received a client packet from the server");
      break;
    }
  }
}

void cl_parse_client_info(const frame_t *frame)
{
  // NOTE: in early phases of network development, the client sent usercmds
  // before the socket connection was properly established. Because the socket
  // was TCP based and the implementation was based off that, it did not
  // properly handle it. I think removing it now might work now but might as
  // well leave it in. I should instead create a handshake exchange but it
  // seems kinda redundant. That or I should create a 'map change' request from
  // the server.
  
  if (cl.connection == CONN_CONNECTING) {
    cl.connection = CONN_CONNECTED;
    log_printf(LOG_DEBUG, "connected");
  }
  
  cg.ent_client = frame->data.client_info.entity;
  
  cl_load_map(frame->data.client_info.map_name);
}

void cl_parse_snapshot(const frame_t *frame)
{
  cl.snapshot_ack = frame->data.snapshot.seq;
  cl.cmd_tail = frame->data.snapshot.ack;
  cl.snapshot = frame->data.snapshot.d;
}

void cl_parse_chat(const frame_t *frame)
{
  // NOTE: no null termination check here might cause problems.
  printf("%s\n", frame->data.chat.content);
}

void cl_send_cmd()
{
  if (cl.connection == CONN_DISCONNECTED)
    return;
  
  frame_t frame;
  frame.netcmd = NETCMD_USERCMD;
  frame.data.usercmd.ack = cl.snapshot_ack;
  frame.data.usercmd.d = cl.usercmd;
  
  frame_send(cl.sock, &frame);
  
  // Queue the usercmd for prediction
  cl.cmd_queue[cl.cmd_head++ % MAX_CMD_QUEUE] = cl.usercmd;
  
  // Because the usercmd cache is a fixed array, too many of them may cause
  // problems.
  // Also, at the moment there are no network disconnect checks. This is as the
  // main and only server is a dedicated one so it does not expect a disconnect.
  // If the server does disconnect, the client sends usercmds without them ever
  // being acknowledged leading to this being spammed.
  if (cl.cmd_head - cl.cmd_tail >= MAX_CMD_QUEUE) {
    log_printf(
      LOG_WARNING,
      "cl_send_cmd(): too many usercmds %i/%i %i %i",
      cl.cmd_head - cl.cmd_tail,
      MAX_CMD_QUEUE,
      cl.cmd_tail,
      cl.cmd_head);
  }
}

void cl_send_chat(const char *text)
{
  if (cl.connection == CONN_DISCONNECTED)
    return;
  
  frame_t frame;
  frame.netcmd = NETCMD_CHAT;
  strncpy(frame.data.chat.content, text, sizeof(frame.data.chat.content));
  
  frame_send(cl.sock, &frame);
}

void cl_send_score()
{
  if (cl.connection == CONN_DISCONNECTED)
    return;
  
  frame_t frame;
  frame.netcmd = NETCMD_SCORE;
  frame_send(cl.sock, &frame);
}

void cl_send_name(const char *name)
{
  if (cl.connection == CONN_DISCONNECTED)
    return;
  
  frame_t frame;
  frame.netcmd = NETCMD_NAME;
  strncpy(frame.data.name.name, name, sizeof(frame.data.name.name));
  
  frame_send(cl.sock, &frame);
}
