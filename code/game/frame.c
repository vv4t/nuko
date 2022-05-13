#include "frame.h"

static const int frame_data_size[] = {
  sizeof(net_client_info_t),
  sizeof(net_usercmd_t),
  sizeof(net_snapshot_t),
  sizeof(net_name_t),
  sizeof(net_chat_t),
  0
};

static const char *frame_data_name[] = {
  "net_client_info_t",
  "net_usercmd_t",
  "net_snapshot_t",
  "net_name_t",
  "net_chat_t",
  "net_score_t"
};

int frame_read(sock_t sock, frame_t *frame)
{
  int read = net_sock_read(sock, &frame->netcmd, sizeof(frame->netcmd));
  
  if (read > 0) {
    do {
      if (frame_data_size[frame->netcmd] > 0)
        read = net_sock_read(sock, &frame->data, frame_data_size[frame->netcmd]);
    } while (read == -1 && read != 0);
  }
  
  return read;
}

void frame_send(sock_t sock, frame_t *frame)
{
  net_sock_send(sock, frame, sizeof(frame->netcmd) + frame_data_size[frame->netcmd]);
}
