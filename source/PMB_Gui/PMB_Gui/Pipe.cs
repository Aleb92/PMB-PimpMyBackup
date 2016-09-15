using System;
using System.Collections.Generic;
using System.IO.Pipes;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Threading;

namespace PMB_Gui
{
    public class Pipe
    {

        public enum pipe_codes : byte
        {
            WRONG_CREDENTIALS = 1,
            WORK_COUNT = 2,
            FILE_VERSION = 3
        }

        private const int interval = 1000;

        public event Action InvalidLogin;
        public event Action<int> WorkingCount;

        private NamedPipeClientStream pipeStream;
        private DispatcherTimer timer = new DispatcherTimer
        {
            Interval = TimeSpan.FromMilliseconds(interval)
        };
        private byte[] buffer = new byte[4];

        public Pipe(string pipeName)
        {
            //timer.Tick += peek_workers;
            pipeStream = new NamedPipeClientStream("PMB");
            pipeStream.Connect();
            //pipeStream.BeginRead(buffer, 0, 1, new AsyncCallback(recieved), null);
        }

        private void recieved(IAsyncResult ar)
        {
            if (ar.IsCompleted && pipeStream.EndRead(ar) == 1)
            {
                switch ((pipe_codes)buffer[0])
                {
                    case pipe_codes.WRONG_CREDENTIALS:
                        InvalidLogin();
                        break;
                    case pipe_codes.WORK_COUNT:
                        //Qui devo ancora leggere un int32
                        pipeStream.Read(buffer, 0, 4);
                        WorkingCount(BitConverter.ToInt32(buffer, 0));
                        break;
                }
                pipeStream.BeginRead(buffer, 0, 1, new AsyncCallback(recieved), null);
            }
        }

        private void peek_workers(object sender, EventArgs e) {
            pipeStream.WriteByte((byte)pipe_codes.WORK_COUNT);
            pipeStream.Flush();
        }

        public void selectVersion(string filename, long timestamp)
        {
            pipeStream.WriteByte((byte)pipe_codes.FILE_VERSION);
            pipeStream.Write(BitConverter.GetBytes(filename.Length), 0, 4);
            byte[] str = Encoding.Unicode.GetBytes(filename);
            pipeStream.Write(str, 0, str.Length);
            pipeStream.Write(BitConverter.GetBytes(filename.Length), 0, filename.Length*2);
            pipeStream.Write(BitConverter.GetBytes(timestamp), 0, 8);
        }
    }
}
