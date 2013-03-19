using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO.Ports;
using System.Windows.Forms;
using System.Diagnostics;

namespace SerialBootloaderDataSource
{
    public class SerialCommandProcessor
    {
        public const int ReceiveBufferSize = 256;
        public const int TransmitBufferSize = 514;

        // Response codes (see F50x_Master_Interface.h)
        public const byte SRC_RSP_OK = 0x70;
        public const byte SRC_RSP_ERROR = 0x71;
        public const byte SRC_RSP_DATA_END = 0x72;
        public const byte SRC_RSP_UNKNOWN_CMD = 0x73;

        SerialPort mSerialPort = new SerialPort();

        string mPortName = "COM1";
        int mBaudRate = 115200;
        int mDataBits = 8;
        Parity mParity = (Parity)Enum.Parse(typeof(Parity), "None");
        StopBits mStopBits = (StopBits)Enum.Parse(typeof(StopBits), "One");
        Handshake mFlowControlType = (Handshake)Enum.Parse(typeof(Handshake), "None");
        int mReadTimeout = SerialPort.InfiniteTimeout;
        int mWriteTimeout = 500; // in ms

        byte[] mReceiveBuffer = new byte[ReceiveBufferSize];
        byte[] mTransmitBuffer = new byte[TransmitBufferSize];

        HexImage mHexImage = null;
        public HexImage HexImage
        {
            get { return mHexImage; }
            set { mHexImage = value; }
        }

        TargetInfo mTargetInfo = null;
        public TargetInfo TargetInfo
        {
            get { return mTargetInfo; }
            set { mTargetInfo = value; }
        }

        bool mUpdateAppFW = false;
        public bool UpdateAppFW
        {
            get { return mUpdateAppFW; }
            set { mUpdateAppFW = value; }
        }

        bool mResponseToMasterPending = false;
        public bool ResponseToMasterPending
        {
            get { return mResponseToMasterPending; }
            set { mResponseToMasterPending = value; }
        }

        Button mOpenCloseButton = null;

        /// <summary>
        /// Constructor
        /// </summary>
        public SerialCommandProcessor(HexImage hexImage, TargetInfo targetInfo, Button openCloseButton)
        {
            mHexImage = hexImage;
            mTargetInfo = targetInfo;
            mOpenCloseButton = openCloseButton;

            // Add Event Handler
            mSerialPort.DataReceived += new SerialDataReceivedEventHandler(SerialPort_DataReceived);
        }

        public string PortName
        {
            get { return mPortName; }
            set { mPortName = value; }
        }

        public string BaudRate
        {
            get { return mBaudRate.ToString(); }
            set { mBaudRate = int.Parse(value); }
        }

        public string DataBits
        {
            get { return mDataBits.ToString(); }
            set { mDataBits = int.Parse(value); }
        }
        
        public string ParityType
        {
            get { return mParity.ToString(); }
            set { mParity = (Parity)Enum.Parse(typeof(Parity), value); }
        }        

        public string StopBits
        {
            get { return mStopBits.ToString(); }
            set { mStopBits = (StopBits)Enum.Parse(typeof(StopBits), value); }
        }

        public string FlowControlType
        {
            get { return mFlowControlType.ToString(); }
            set { mFlowControlType = (Handshake)Enum.Parse(typeof(Handshake), value); }
        }

        public int ReadTimeout
        {
            get { return mReadTimeout; }
            set { mReadTimeout = value; }
        }

        public int WriteTimeout
        {
            get { return mWriteTimeout; }
            set { mWriteTimeout = value; }
        }

        public void ListAvailableSerialPorts(ComboBox cbox)
        {
            foreach (string pname in SerialPort.GetPortNames())
            {
                cbox.Items.Add(pname);
            }
            cbox.SelectedIndex = 0;
        }

        public void ListBaudRateOptions(ComboBox cbox)
        {
            string[] baudRateList = {"1200", "2400", "9600", "14400", "28800",
                                      "57600", "115200", "230400"};

            foreach (string str in baudRateList)
            {
                cbox.Items.Add(str);
            }
            cbox.SelectedIndex = 6; // 115200
            cbox.Enabled = false;
        }

        public void ListDataBitsOptions(ComboBox cbox)
        {
            string[] dataBitsList = {"7", "8", "9"};

            foreach (string str in dataBitsList)
            {
                cbox.Items.Add(str);
            }
            cbox.SelectedIndex = 1; // "8" data bits
            cbox.Enabled = false;
        }

        public void ListParityOptions(ComboBox cbox)
        {
            foreach (string str in Enum.GetNames(typeof(Parity)))
            {
                cbox.Items.Add(str);
            }
            cbox.SelectedIndex = 0; // "None"
            cbox.Enabled = false;            
        }

        public void ListStopBitsOptions(ComboBox cbox)
        {
            foreach (string str in Enum.GetNames(typeof(StopBits)))
            {
                cbox.Items.Add(str);
            }
            cbox.SelectedIndex = 1; // "1" stop bit
            cbox.Enabled = false;
        }

        public void ListFlowControlOptions(ComboBox cbox)
        {
            foreach (string str in Enum.GetNames(typeof(Handshake)))
            {
                cbox.Items.Add(str);
            }
            cbox.SelectedIndex = 0; // "None"
            cbox.Enabled = false;
        }

        public bool IsPortOpen
        {
            get { return mSerialPort.IsOpen; }
        }

        public void PortOpenClose()
        {
            if (mSerialPort.IsOpen == false)
            {
                if (OpenSerialPort() == true)
                {
                    mOpenCloseButton.Text = "Close COM Port";
                }
            }
            else
            {
                FinishPriorPendingRequest(false);
                CloseSerialPort();
                mOpenCloseButton.Text = "Open COM Port";
            }
        }

        public bool OpenSerialPort()
        {
            bool retval = true;

            mSerialPort.BaudRate = mBaudRate;
            mSerialPort.DataBits = mDataBits;
            mSerialPort.Parity = mParity;
            mSerialPort.StopBits = mStopBits;
            mSerialPort.Handshake = mFlowControlType;

            mSerialPort.ReadTimeout = mReadTimeout;
            mSerialPort.WriteTimeout = mWriteTimeout;

            mSerialPort.PortName = mPortName;

            try
            {
                mSerialPort.Open();
                mSerialPort.DiscardInBuffer();

                Debug.WriteLine("Port opened: " + mSerialPort.PortName + Environment.NewLine + Environment.NewLine);
                DispatchUpdateDisplayEvent(DisplayManager.OutputStyle.Status, DisplayManager.OutputLevel.AlwaysShow, "Port opened: " + PortName + Environment.NewLine + Environment.NewLine);
                DispatchUpdateDisplayEvent(DisplayManager.OutputStyle.Status, DisplayManager.OutputLevel.AlwaysShow, "Waiting for commands from the Master MCU..." + Environment.NewLine + Environment.NewLine);
            }
            catch (Exception ex)
            {
                retval = false;
                Debug.WriteLine("Error: Could not open serial port. Original error: " + ex.Message + Environment.NewLine);
                DispatchUpdateDisplayEvent(DisplayManager.OutputStyle.Error, DisplayManager.OutputLevel.AlwaysShow, "Error: Could not open serial port. Original error: " + ex.Message + Environment.NewLine + Environment.NewLine);
            }
            return retval;
        }

        public void CloseSerialPort()
        {
            if (mSerialPort.IsOpen == true)
            {
                mSerialPort.Close();
                Debug.WriteLine("Port closed: " + mSerialPort.PortName + Environment.NewLine + Environment.NewLine);
                DispatchUpdateDisplayEvent(DisplayManager.OutputStyle.Status, DisplayManager.OutputLevel.AlwaysShow, "Port closed: " + PortName + Environment.NewLine + Environment.NewLine);
            }
        }

        /// <summary>
        /// Writes specified number of bytes from the 
        /// Transmit buffer to the serial port.
        /// </summary>
        /// <param name="numBytes"></param>
        public void WriteToSerialPort(int numBytes)
        {
            if (mSerialPort.IsOpen == true)
            {
                try
                {
                    mSerialPort.Write(mTransmitBuffer, 0, numBytes);
                    DispatchUpdateDisplayEvent(DisplayManager.OutputStyle.TX, DisplayManager.OutputLevel.RawData, "Sent [Hex]: " + BitConverter.ToString(mTransmitBuffer, 0, numBytes) + Environment.NewLine + Environment.NewLine);
                }
                catch (Exception ex)
                {
                    DispatchUpdateDisplayEvent(DisplayManager.OutputStyle.Error, DisplayManager.OutputLevel.AlwaysShow, "Error: Could not write to serial port. Original error: " + ex.Message + Environment.NewLine + Environment.NewLine);
                }
            }
        }

        public struct DataSourceCommand
        {
            public string commandName;
            public byte commandCode;
            public int commandRxSize;
            public int commandTxSize;

            public DataSourceCommand(string cn, byte cc, int crs, int cts)
            {
                commandName = cn;
                commandCode = cc;
                commandRxSize = crs;
                commandTxSize = cts;
            }
        }

        public DataSourceCommand[] DScommand = 
        {
            new DataSourceCommand("GetHexImageInfo", 0x80, 1, 14),
            new DataSourceCommand("GetPageInfo", 0x81, 1, 6),
            new DataSourceCommand("GetPage", 0x82, 1, 514),
            new DataSourceCommand("DisplayTargetInfo", 0x83, 19, 1),
            new DataSourceCommand("DisplayInfoCode", 0x84, 2, 1),
            new DataSourceCommand("GetSeqFlashBytes", 0x11, 2, TransmitBufferSize),
            new DataSourceCommand("GetRunningCRC", 0x12, 1, 2),
            new DataSourceCommand("ClosePort", 0x13, 2, 0)
        };

        public enum DScommandSeq
        {
            GetHexImageInfo = 0,
            GetPageInfo,
            GetPage,
            DisplayTargetInfo,
            DisplayInfoCode,
            GetSeqFlashBytes,
            GetRunningCRC,
            ClosePort,
            EndValue
        }


        int mTotalBytesToReceive = 0;
        bool mAwaitCommandCode = true;
        int mCommandCode = 0;

        uint mCurrentAddr = 0;
        UInt16 mCurrentCRC = 0;


        int mValidPageCount = 0; 

        int mCurrentPageIndex = 0;
        public int CurrentPageStartAddr
        {
            get { return ComputePageAddrFromPageIndex(); }
        }

        int ComputePageAddrFromPageIndex()
        {
            int pageStartAddr;
            if (mHexImage.FlashPageSize == 512)
            {
                pageStartAddr = mCurrentPageIndex << 9;
            }
            else
            {
                pageStartAddr = mCurrentPageIndex << 10;
            }
            return pageStartAddr;
        }

        /// <summary>
        /// This event handler is called whenever new data 
        /// has been received by the serial port.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        void SerialPort_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            try
            {
                int bytesAvailable = mSerialPort.BytesToRead;

                if (mAwaitCommandCode == true)
                {
                    if (bytesAvailable >= 1)
                    {
                        mSerialPort.Read(mReceiveBuffer, 0, 1);
                        if (ValidateCommandCode(mReceiveBuffer[0]) == true)
                        {
                            mAwaitCommandCode = false;
                        }
                    }
                }

                // Update currently available number of bytes
                bytesAvailable = mSerialPort.BytesToRead;

                // If sufficient bytes are available, go ahead and read them
                if (mAwaitCommandCode == false && ((bytesAvailable >= mTotalBytesToReceive) || (mTotalBytesToReceive == 0)))
                {
                    if (mTotalBytesToReceive > 0)
                    {
                        mSerialPort.Read(mReceiveBuffer, 0, mTotalBytesToReceive);
                        DispatchUpdateDisplayEvent(DisplayManager.OutputStyle.RX, DisplayManager.OutputLevel.RawData, "Received [Hex]: " + BitConverter.ToString(mReceiveBuffer, 0, mTotalBytesToReceive) + Environment.NewLine + Environment.NewLine);
                    }
                    mAwaitCommandCode = true;   // setup for receiving next command
                    ProcessCommand();   // process current command
                }
                else
                {
                    // If fewer bytes than needed are available, postpone the read for later.
                }
            }
            catch (Exception ex)
            {
                Debug.WriteLine("Error reading from COM port. Original error: " + ex.Message);
                DispatchUpdateDisplayEvent(DisplayManager.OutputStyle.Error, DisplayManager.OutputLevel.AlwaysShow, "Error reading from COM port. Original error: " + ex.Message + Environment.NewLine + Environment.NewLine);
            }
        }

        bool ValidateCommandCode(byte rxbyte)
        {
            int i = 0;
            bool commandOK = false;

            // Iterate through all the commands for a match
            for(i = DScommand.GetLowerBound(0); i <= DScommand.GetUpperBound(0); i++)
            {
                if (rxbyte == DScommand[i].commandCode)
                {
                    commandOK = true;
                    mCommandCode = rxbyte;
                    DispatchUpdateDisplayEvent(DisplayManager.OutputStyle.Status, DisplayManager.OutputLevel.Verbose, "Received Command '" + DScommand[i].commandName + "' [0x" + BitConverter.ToString(mReceiveBuffer, 0, 1) + "]" + Environment.NewLine);
                    mTotalBytesToReceive = DScommand[i].commandRxSize - 1;
                    break;
                }
            }

            if(commandOK == false)
            {
                DispatchUpdateDisplayEvent(DisplayManager.OutputStyle.Error, DisplayManager.OutputLevel.AlwaysShow, Environment.NewLine + "Error: Invalid Command Code" + Environment.NewLine + Environment.NewLine);
            }

            return commandOK;
        }

        void ProcessCommand()
        {
            if(mCommandCode == DScommand[(int)DScommandSeq.DisplayTargetInfo].commandCode)
            {
                mTargetInfo.InitTargetInfo(mReceiveBuffer, 0);
                DispatchUpdateDisplayEvent(DisplayManager.OutputStyle.TargetInfo, DisplayManager.OutputLevel.AlwaysShow,  Environment.NewLine + "Received Target MCU Information." + Environment.NewLine + "See table for details." + Environment.NewLine + Environment.NewLine);

                // Note: No response will be sent to master right now. The user is given the opportunity to
                // decide if firmware update should proceed. Response will be sent once the user makes a selection
                // by calling ContinueBootloadProcess()

                mResponseToMasterPending = true;
                DispatchUpdateDisplayEvent(DisplayManager.OutputStyle.Error, DisplayManager.OutputLevel.AlwaysShow, "Click the 'Update Application Firmware' button to continue." + Environment.NewLine + Environment.NewLine);
            }
            else if (mCommandCode == DScommand[(int)DScommandSeq.DisplayInfoCode].commandCode)
            {
                DispatchUpdateDisplayEvent(DisplayManager.OutputStyle.Error, DisplayManager.OutputLevel.AlwaysShow, "Error Code: 0x" + mReceiveBuffer[0].ToString("X2") + Environment.NewLine);
                mTransmitBuffer[0] = SRC_RSP_OK;
                WriteToSerialPort(DScommand[(int)DScommandSeq.DisplayInfoCode].commandTxSize);
            }
            else if(mCommandCode == DScommand[(int)DScommandSeq.GetHexImageInfo].commandCode)
            {
                if (mHexImage.HexImageValid == true)
                {
                    mHexImage.LoadHexImageInfoResponse(ref mTransmitBuffer);

                    // Reset internal addr and crc variables
                    mCurrentAddr = HexImage.AppFWstartAddr;
                    mCurrentCRC = 0;

                    // Reset internal page index
                    mCurrentPageIndex = 0;
                    mValidPageCount = 0;
                }
                else
                {
                    mTransmitBuffer[0] = SRC_RSP_ERROR;
                }

                WriteToSerialPort(DScommand[(int)DScommandSeq.GetHexImageInfo].commandTxSize);
            }
            else if (mCommandCode == DScommand[(int)DScommandSeq.GetPageInfo].commandCode)
            {
                if (mHexImage.HexImageValid == true)
                {
                    do
                    {
                        if (mHexImage.PageSpecified512[mCurrentPageIndex] == true)
                        {
                            break;
                        }
                        else
                        {
                            mCurrentPageIndex++;
                        }
                    }
                    while (CurrentPageStartAddr <= mHexImage.AppFWendAddr);

                    if (CurrentPageStartAddr <= mHexImage.AppFWendAddr)
                    {
                        mTransmitBuffer[0] = SRC_RSP_OK;
                        mTransmitBuffer[1] = (byte)(CurrentPageStartAddr & 0xFF);
                        mTransmitBuffer[2] = (byte)((CurrentPageStartAddr & 0xFF00) >> 8);
                        mTransmitBuffer[3] = (byte)((CurrentPageStartAddr & 0xFF0000) >> 16);

                        byte[] crcBytes = BitConverter.GetBytes(mHexImage.GetHexImagePartialCRC(
                                CurrentPageStartAddr, (int)mHexImage.FlashPageSize));

                        mTransmitBuffer[4] = crcBytes[0];
                        mTransmitBuffer[5] = crcBytes[1];

                        mValidPageCount++;
                    }
                    else
                    {
                        // No more pages to send
                        mTransmitBuffer[0] = SRC_RSP_DATA_END;
                        mTransmitBuffer[1] = (byte)((mValidPageCount-1)& 0xFF);
                        // In this case, the other bytes in the message can be anything

                        DispatchUpdateDisplayEvent(DisplayManager.OutputStyle.Status, DisplayManager.OutputLevel.AlwaysShow, Environment.NewLine + "Bootload process completed successfully!" + Environment.NewLine + Environment.NewLine);
                        DispatchUpdateDisplayEvent(DisplayManager.OutputStyle.Status, DisplayManager.OutputLevel.AlwaysShow, "Waiting for commands from the Master MCU..." + Environment.NewLine + Environment.NewLine);
                    }
                }
                else
                {
                    mTransmitBuffer[0] = SRC_RSP_ERROR;
                }

                WriteToSerialPort(DScommand[(int)DScommandSeq.GetPageInfo].commandTxSize);
            }
            else if (mCommandCode == DScommand[(int)DScommandSeq.GetPage].commandCode)
            {
                if (mHexImage.HexImageValid == true)
                {
                    mTransmitBuffer[0] = SRC_RSP_OK;
                    mHexImage.ExtractHexImageBytes(ref mTransmitBuffer, 1, (uint)CurrentPageStartAddr, (int)mHexImage.FlashPageSize);
                    mTransmitBuffer[mHexImage.FlashPageSize+1] = SRC_RSP_OK;

                    mCurrentPageIndex++; // Increment current page index to prepare for next request
                }
                else
                {
                    mTransmitBuffer[0] = SRC_RSP_ERROR;
                }
                WriteToSerialPort((int)mHexImage.FlashPageSize + 2);
            }
            else if(mCommandCode == DScommand[(int)DScommandSeq.GetSeqFlashBytes].commandCode)
            {
                // TODO: Implement HexImageValid check and other items.
                mHexImage.ExtractHexImageBytes(ref mTransmitBuffer, 0, mCurrentAddr, TargetInfo.BLbufferSize);
                WriteToSerialPort(TargetInfo.BLbufferSize);
            }
            else if(mCommandCode == DScommand[(int)DScommandSeq.GetRunningCRC].commandCode)
            {
                // TODO: Implement HexImageValid check and other items.
                mTransmitBuffer[0] = (byte)(mCurrentCRC & 0xFF);
                mTransmitBuffer[1] = (byte)((mCurrentCRC & 0xFF00)>>8);
                WriteToSerialPort(DScommand[(int)DScommandSeq.GetRunningCRC].commandTxSize);
                
                // Clear running CRC after it has been read by the Master
                mCurrentCRC = 0;
            }
            else if(mCommandCode == DScommand[(int)DScommandSeq.ClosePort].commandCode)
            {
                // TODO: Implement HexImageValid check and other items.
                if (mSerialPort.IsOpen == true)
                {
                    PortOpenClose(); // Close port if it is open
                }
                DispatchUpdateDisplayEvent(DisplayManager.OutputStyle.Status, DisplayManager.OutputLevel.AlwaysShow, Environment.NewLine + "Bootload process completed successfully!" + Environment.NewLine + Environment.NewLine);
            }
        }

        public void FinishPriorPendingRequest(bool flag)
        {
            if (ResponseToMasterPending == true)
            {
                if (flag == true)
                {
                    mTransmitBuffer[0] = SRC_RSP_OK;
                }
                else
                {
                    mTransmitBuffer[0] = SRC_RSP_ERROR;
                }

                WriteToSerialPort(DScommand[(int)DScommandSeq.DisplayTargetInfo].commandTxSize);

                DispatchUpdateDisplayEvent(DisplayManager.OutputStyle.Status, DisplayManager.OutputLevel.AlwaysShow, Environment.NewLine + "Starting the bootload process..." + Environment.NewLine + Environment.NewLine);
                
                mResponseToMasterPending = false;
            }
        }

        #region EventDispatcher

        /// <summary>
        /// Custom event "UpdateDisplay".
        /// </summary>
        public event UpdateDisplayDelegate UpdateDisplay;

        public void SubScribeUpdateDisplayEvent(UpdateDisplayDelegate eventHandler)
        {
            UpdateDisplay += eventHandler;
        }

        public void UnsubscribeUpdateDisplayEvent(UpdateDisplayDelegate eventHandler)
        {
            UpdateDisplay -= eventHandler;
        }

        private void DispatchUpdateDisplayEvent(DisplayManager.OutputStyle messageType, DisplayManager.OutputLevel messageLevel, string message)
        {
            // Check if there is at least one subscriber for this event before raising the event
            if (UpdateDisplay != null)
            {
                UpdateDisplay(new UpdateDisplayEventArgs(messageType, messageLevel, message));
            }
        }

        #endregion
    }

    public class UpdateDisplayEventArgs : EventArgs
    {
        DisplayManager.OutputStyle mMessageType;
        DisplayManager.OutputLevel mMessageLevel;
        string mMessage;

        public DisplayManager.OutputStyle MessageType
        {
            get { return mMessageType; }
        }

        public DisplayManager.OutputLevel MessageLevel
        {
            get { return mMessageLevel; }
        }

        public string Message
        {
            get { return mMessage; }
        }

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="messageType"></param>
        /// <param name="message"></param>
        /// <param name="receiveBuf"></param>
        /// <param name="transmitBuf"></param>
        public UpdateDisplayEventArgs(DisplayManager.OutputStyle messageType, DisplayManager.OutputLevel messageLevel, string message)
        {
            mMessageType = messageType;
            mMessageLevel = messageLevel;
            mMessage = message;
        }
    }

    /// <summary>
    /// This is the delegate whose signature should match all 
    /// handlers for this event.
    /// </summary>
    /// <param name="?"></param>
    public delegate void UpdateDisplayDelegate(UpdateDisplayEventArgs e);
}