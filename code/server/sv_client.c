#include "sv_local.h"

entity_t sv_new_client(sock_t sock)
{
  entity_t entity = edict_add_entity(&sv.edict, SV_ES_CLIENT);
  
  sv.bg.transform[entity] = (bg_transform_t) {0};
  sv.bg.motion[entity] = (bg_motion_t) {0};
  
  sv.bg.transform[entity].position.y = 3;
  
  sv.bg.capsule[entity].radius = 0.5f;
  sv.bg.capsule[entity].height = 1.0f;
  sv.bg.model[entity] = BG_MDL_FUMO_CIRNO;
  
  sv.client[entity].sock = sock;
  sv.client[entity].cmd_tail = 0;
  sv.client[entity].cmd_head = 0;
  sv.client[entity].snapshot_ack = 0;
  snprintf(sv.client[entity].name, sizeof(sv.client[entity].name), "GUEST%i", (rand() % 100000) + 100000);
  
  return entity;
}

void sv_client_parse_frame(entity_t entity, const frame_t *frame)
{
  switch (frame->netcmd) {
  case NETCMD_CLIENT_INFO:
    break;
  case NETCMD_SNAPSHOT:
    break;
  case NETCMD_USERCMD:
    sv_client_parse_usercmd(entity, frame);
    break;
  case NETCMD_CHAT:
    sv_client_parse_chat(entity, frame);
    break;
  }
}

void sv_client_parse_chat(entity_t entity, const frame_t *frame)
{
  const char *content = frame->data.chat.content;
  
  if (content[0] == '/') {
    char chat_cmd[sizeof(frame->data.chat.content)];
    strcpy(chat_cmd, content);
    
    char *c_argv[8];
    int c_argc = 0;
    
    char *str_cmd = chat_cmd;
    while ((c_argv[c_argc] = strtok_r(str_cmd, " ", &str_cmd)))
      c_argc++;
    
    if (strcmp(c_argv[0], "/name") == 0) {
      if (c_argc != 2) {
        sv_client_send_chat(entity, "[SERVER] usage: /name [name]");
        return;
      }
      
      strncpy(sv.client[entity].name, c_argv[1], sizeof(sv.client[entity].name));
      sv_client_send_chat(entity, "[SERVER] your name has been changed.");
    }
  } else {
    char msg[128];
    snprintf(msg, 128, "[CHAT] %s: %s", sv.client[entity].name, frame->data.chat.content);
    
    sv_send_chat(msg);
  }
}

void sv_client_parse_usercmd(entity_t entity, const frame_t *frame)
{
  sv_client_t *svc = &sv.client[entity];
  
  svc->cmd_queue[svc->cmd_head++ % MAX_CMD_QUEUE] = frame->data.usercmd.d;
  if (svc->cmd_head - svc->cmd_tail >= MAX_CMD_QUEUE)
    svc->cmd_tail = svc->cmd_head - MAX_CMD_QUEUE + 1;
  
  svc->snapshot_ack = frame->data.usercmd.ack;
}

void sv_client_send_client_info(entity_t entity)
{
  frame_t frame;
  frame.netcmd = NETCMD_CLIENT_INFO;
  frame.data.client_info.entity = entity;
  
  strcpy(frame.data.client_info.map_name, sv.map_name);
  
  net_sock_send(sv.client[entity].sock, &frame, sizeof(frame_t));
}

void sv_client_send_chat(entity_t entity, const char *text)
{
  frame_t frame;
  frame.netcmd = NETCMD_CHAT;
  strncpy(frame.data.chat.content, text, sizeof(frame.data.chat.content));
  net_sock_send(sv.client[entity].sock, &frame, sizeof(frame_t));
}
