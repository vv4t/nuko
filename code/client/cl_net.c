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
  printf("%s\n", frame->data.chat.content);
}

void cl_send_frame(const frame_t *frame)
{
  
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
  
  cl.cmd_queue[cl.cmd_head++ % MAX_CMD_QUEUE] = cl.usercmd;
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
