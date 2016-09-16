using System;
using System.Collections.Generic;
using System.IO.Pipes;
using System.Linq;
using System.Text;
using System.Threading;
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

        private const int interval = 10000;

        public event Action InvalidLogin;
        public event Action<int> WorkingCount;

        private NamedPipeClientStream pipeStream;
        private Timer timer;
        private byte[] buffer = new byte[4];

        public Pipe(string pipeName)
        {
            pipeStream = new NamedPipeClientStream(".", App.CurrentApp.settings.pipeName, 
                PipeDirection.InOut, PipeOptions.Asynchronous | PipeOptions.WriteThrough);
            pipeStream.Connect();
            pipeStream.BeginRead(buffer, 0, 1, recieved, null);
            timer = new Timer(peek_workers, null, interval, interval);
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
                pipeStream.BeginRead(buffer, 0, 1, recieved, null);
            }
        }

        private void peek_workers(object sender) {
            pipeStream.WriteByte((byte)pipe_codes.WORK_COUNT);
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

        ~Pipe()
        {
            pipeStream.Close();
        }
    }
}
