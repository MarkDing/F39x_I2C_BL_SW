using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO.Ports;
using System.Windows.Forms;
using System.Drawing;

namespace SerialBootloaderDataSource
{
    public class DisplayManager
    {
        public enum OutputLevel { AlwaysShow, Verbose, RawData };
        public enum OutputStyle { Status, TX, RX, Error, TargetInfo };
        Color[] mOutputColor = { Color.Green, Color.Black, Color.Blue, Color.Red };

        RichTextBox mTextDisplayBox = null;
        bool mVerbose = true;
        bool mShowRawData = false;

        DataGridView mGridDisplayBox = null;
        public enum FieldType
        {
            MCUcode, BLtype, FlashPageSize,
            AppFWver, CANdevAddr, 
            AppStartAddr, AppEndAddr,
            BLfwVer, BLbufferSize,
            CRCtype
        };
        string[] mFieldName = 
        { 
            "MCU Code", "BL Type", "Flash Page Size",
            "App FW Version", "SMBUS Device Addr",
            "App Start Addr", "App End Addr" ,
            "Reseved", "Reserved","Reserved"
//            "BL FW Version", "BL Buffer Size", "CRC Type"
        };
        const int cGridNumRows = 10;

        HexImage mHexImage = null;

        SerialCommandProcessor mSerialCommProc = null;

        /// <summary>
        /// Constructor
        /// </summary>
        public DisplayManager(RichTextBox rtb, DataGridView dgv, HexImage hi, SerialCommandProcessor scp)
        {
            mTextDisplayBox = rtb;
            mGridDisplayBox = dgv;
            mHexImage = hi;
            mSerialCommProc = scp;

            InitGridDisplayBox();
        }

        // Properties
        public RichTextBox TextDisplayBox
        {
            get { return mTextDisplayBox; }
            set { mTextDisplayBox = value; }
        }

        public bool Verbose
        {
            get { return mVerbose; }
            set { mVerbose = value; }
        }

        public bool ShowRawData
        {
            get { return mShowRawData; }
            set { mShowRawData = value; }
        }

        public DataGridView GridDisplayBox
        {
            get { return mGridDisplayBox; }
            set { mGridDisplayBox = value; }
        }

        public HexImage HexImage
        {
            get { return mHexImage; }
            set { mHexImage = value; }
        }

        public SerialCommandProcessor SerialCommProc
        {
            get { return mSerialCommProc; }
            set { mSerialCommProc = value; }
        }

        public void InitGridDisplayBox()
        {
            mGridDisplayBox.Rows.Add(cGridNumRows);

            for (int i = 0; i < cGridNumRows; i++)
            {
                mGridDisplayBox.Rows[i].Cells[0].Value = mFieldName[i];
            }

            ClearHexImageInfoInGrid();
            ClearTargetInfoInGrid();

            mGridDisplayBox.ClearSelection(); // This is to clear any cell from being selected
        }

        public void ClearHexImageInfoInGrid()
        {
            for (int i = 0; i < cGridNumRows; i++)
            {
                mGridDisplayBox.Rows[i].Cells[1].Value = "No Data";
            }
        }

        public void ClearTargetInfoInGrid()
        {
            for (int i = 0; i < cGridNumRows; i++)
            {
                mGridDisplayBox.Rows[i].Cells[2].Value = "No Data";
            }
        }

        public void DisplayHexImageInfoInGrid()
        {
            mGridDisplayBox.Rows[(int)FieldType.MCUcode].Cells[1].Value = mHexImage.MCUcode.ToString("X2");
            mGridDisplayBox.Rows[(int)FieldType.BLtype].Cells[1].Value = mHexImage.BLtype;
            mGridDisplayBox.Rows[(int)FieldType.FlashPageSize].Cells[1].Value = mHexImage.FlashPageSize.ToString();
            mGridDisplayBox.Rows[(int)FieldType.AppFWver].Cells[1].Value = mHexImage.AppFWversion;
            mGridDisplayBox.Rows[(int)FieldType.CANdevAddr].Cells[1].Value = mHexImage.CANdeviceAddr.ToString("X2");
            mGridDisplayBox.Rows[(int)FieldType.AppStartAddr].Cells[1].Value = mHexImage.AppFWstartAddr.ToString("X6");
            mGridDisplayBox.Rows[(int)FieldType.AppEndAddr].Cells[1].Value = mHexImage.AppFWendAddr.ToString("X6");
        }

        public void DisplayTargetInfoInGrid()
        {
            mGridDisplayBox.Rows[(int)FieldType.MCUcode].Cells[2].Value = mSerialCommProc.TargetInfo.MCUcode.ToString("X2");
            mGridDisplayBox.Rows[(int)FieldType.BLtype].Cells[2].Value = mSerialCommProc.TargetInfo.BLtype;
            mGridDisplayBox.Rows[(int)FieldType.FlashPageSize].Cells[2].Value = mSerialCommProc.TargetInfo.FlashPageSize.ToString();
            mGridDisplayBox.Rows[(int)FieldType.AppFWver].Cells[2].Value = mSerialCommProc.TargetInfo.AppFWversion;
            mGridDisplayBox.Rows[(int)FieldType.CANdevAddr].Cells[2].Value = mSerialCommProc.TargetInfo.CANdeviceAddr.ToString("X2");
            mGridDisplayBox.Rows[(int)FieldType.AppStartAddr].Cells[2].Value = mSerialCommProc.TargetInfo.AppFWstartAddr.ToString("X6");
            mGridDisplayBox.Rows[(int)FieldType.AppEndAddr].Cells[2].Value = mSerialCommProc.TargetInfo.AppFWendAddr.ToString("X6");
            mGridDisplayBox.Rows[(int)FieldType.BLfwVer].Cells[2].Value = mSerialCommProc.TargetInfo.BLfwVersion;
            mGridDisplayBox.Rows[(int)FieldType.BLbufferSize].Cells[2].Value = mSerialCommProc.TargetInfo.BLbufferSize.ToString();
            mGridDisplayBox.Rows[(int)FieldType.CRCtype].Cells[2].Value = mSerialCommProc.TargetInfo.CRCtype;
        }


        public void ClearTextDisplayBox()
        {
            if (mTextDisplayBox != null)
            {
                mTextDisplayBox.Clear();
            }
        }
        
        [STAThread]
        public void PostToTextDisplayBox(OutputStyle style, OutputLevel level, string output)
        {
            if (mTextDisplayBox != null)
            {
                if ((level == OutputLevel.AlwaysShow) || (level == OutputLevel.Verbose && Verbose == true)
                    || (level == OutputLevel.RawData && ShowRawData == true))
                {
                    mTextDisplayBox.Invoke(new EventHandler(delegate
                    {
                        mTextDisplayBox.SelectionFont = new Font(mTextDisplayBox.SelectionFont, FontStyle.Bold);
                        mTextDisplayBox.SelectionColor = mOutputColor[(int)style];
                        mTextDisplayBox.AppendText(output);
                        mTextDisplayBox.ScrollToCaret();
                    }));
                }
            }
        }


        public void HandleUpdateDisplayEvent(UpdateDisplayEventArgs e)
        {
            if (e.MessageType == OutputStyle.TargetInfo)
            {
                DisplayTargetInfoInGrid();
                PostToTextDisplayBox(OutputStyle.Status, e.MessageLevel, e.Message);
            }
            else
            {
                PostToTextDisplayBox(e.MessageType, e.MessageLevel, e.Message);
            }
        }

    }
}
