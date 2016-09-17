using System;
using System.Collections.Generic;
using System.IO.Pipes;
using System.Linq;
using System.Text;
using System.Timers;
using System.Threading.Tasks;
using System.Windows.Threading;

namespace PMB_Gui
{
    public class Pipe : IDisposable
    {

        public enum pipe_codes : byte
        {
            WRONG_CREDENTIALS = 1,
            WORK_COUNT = 2,
            FILE_VERSION = 3,
            CLOSING = 255
        }

        private const int interval = 10000;

        public event Action InvalidLogin;
        public event Action<int> WorkingCount;

        private NamedPipeClientStream pipeStream;
        private IAsyncResult res;
        private Timer timer;
        private byte[] buffer = new byte[4];

        public Pipe(string pipeName)
        {
            pipeStream = new NamedPipeClientStream(".", App.CurrentApp.settings.pipeName, 
                PipeDirection.InOut, PipeOptions.Asynchronous | PipeOptions.WriteThrough);
            pipeStream.Connect();
            res = pipeStream.BeginRead(buffer, 0, 1, recieved, null);
            timer = new Timer(interval)
            {
                AutoReset = true
            };
            timer.Elapsed += peek_workers;
            timer.Start();
        }

        private void recieved(IAsyncResult ar)
        {
            if (pipeStream.EndRead(ar) == 1 && ar.IsCompleted)
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
                    case pipe_codes.CLOSING:
                        timer.Stop();
                        pipeStream.Close();
                        return;
                }
                if(pipeStream.IsConnected)
                    pipeStream.BeginRead(buffer, 0, 1, recieved, null);
            }
        }

        private void peek_workers(object sender, ElapsedEventArgs e) {
            if (pipeStream.IsConnected)
                try
                {
                    pipeStream.WriteByte((byte)pipe_codes.WORK_COUNT);
                }
                catch { }
        }

        public void selectVersion(string filename, long timestamp)
        {
            pipeStream.WriteByte((byte)pipe_codes.FILE_VERSION);
            pipeStream.Write(BitConverter.GetBytes(filename.Length), 0, 4);
            byte[] str = Encoding.Unicode.GetBytes(filename);
            pipeStream.Write(str, 0, str.Length);
            pipeStream.Write(BitConverter.GetBytes(timestamp), 0, 8);
        }
        
    }
}
