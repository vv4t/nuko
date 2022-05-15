#include "cl_local.h"

void cl_connect(const char *host)
{
  cl.sock = net_connect(host);
}

void cl_parse()
{
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
    case NETCMD_SCORE:
      break;
    case NETCMD_USERCMD:
      break;
    case NETCMD_NAME:
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
  
  cl.connected = true;
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
  if (!cl.connected)
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
  if (!cl.connected)
    return;
  
  frame_t frame;
  frame.netcmd = NETCMD_CHAT;
  strncpy(frame.data.chat.content, text, sizeof(frame.data.chat.content));
  
  frame_send(cl.sock, &frame);
}

void cl_send_score()
{
  if (!cl.connected)
    return;
  
  frame_t frame;
  frame.netcmd = NETCMD_SCORE;
  frame_send(cl.sock, &frame);
}

void cl_send_name(const char *name)
{
  if (!cl.connected)
    return;
  
  frame_t frame;
  frame.netcmd = NETCMD_NAME;
  strncpy(frame.data.name.name, name, sizeof(frame.data.name.name));
  
  frame_send(cl.sock, &frame);
}
