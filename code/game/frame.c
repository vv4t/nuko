#include "frame.h"

// Table for how large each data structure is, based on netcmd_t
static const int frame_data_size[] = {
  sizeof(net_client_info_t),
  sizeof(net_usercmd_t),
  sizeof(net_snapshot_t),
  sizeof(net_name_t),
  sizeof(net_chat_t),
  0 // A score request holds no data
};

int frame_read(sock_t sock, frame_t *frame)
{
  // Read the type of frame
  int read = net_sock_read(sock, &frame->netcmd, sizeof(frame->netcmd));
  
  if (read > 0) { // If the connection is still open
    do {
      if (frame_data_size[frame->netcmd] > 0)
        read = net_sock_read(sock, &frame->data, frame_data_size[frame->netcmd]); // Read 'x' bytes based on how big the data structure is
    } while (read == -1 && read != 0); // In case the connection closes while waiting for the frame data
  }
  
  return read;
}

void frame_send(sock_t sock, frame_t *frame)
{
  // Send 'x' bytes based on how big the data structure is
  net_sock_send(sock, frame, sizeof(frame->netcmd) + frame_data_size[frame->netcmd]);
}
