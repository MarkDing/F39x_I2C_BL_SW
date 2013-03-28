using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace SerialBootloaderDataSource
{
    /// <summary>
    /// This class includes the fields common
    /// to the App InfoBlock and the BL InfoBlock
    /// </summary>
    public class InfoBlock
    {
        protected Byte mInfoBlockLength = 0;
        protected Byte mMCUcode = 0;
        protected Byte mBLtype = 0;
        protected Byte mFlashPageSizeCode = 0;
        protected Byte mAppFWversionLow = 0;
        protected Byte mAppFWversionHigh = 0;

        protected Byte mCANdeviceAddr = 0;

        protected UInt32 mAppFWstartAddr = 0;
        protected UInt32 mAppFWendAddr = 0;

        /// <summary>
        /// Constructor
        /// </summary>
        public InfoBlock()
        {
        }

        public Byte InfoBlockLength
        {
            get { return mInfoBlockLength; }
        }

        public Byte MCUcode
        {
            get { return mMCUcode; }
        }

        public Byte BLtypeRaw
        {
            get { return mBLtype; }
        }

        public string BLtype
        {
            get { return DecodeBLtype(); }
        }

        string DecodeBLtype()
        {
            string retval;

            switch (mBLtype)
            {
                case 1:
                    retval = "UART";
                    break;
                case 2:
                    retval = "CAN";
                    break;
                case 3:
                    retval = "LIN";
                    break;
                case 4:
                    retval = "SMBUS";
                    break;
                default:
                    retval = "Invalid";
                    break;
            }

            return retval;
        }

        public Byte FlashPageSizeCodeRaw
        {
            get { return mFlashPageSizeCode; }
        }

        public UInt32 FlashPageSize
        {
            get { return DecodeFlashPageSizeCode(); }
        }

        UInt32 DecodeFlashPageSizeCode()
        {
            UInt32 retval;

            switch (mFlashPageSizeCode)
            {
                case 1:
                    retval = 512;
                    break;
                case 2:
                    retval = 1024;
                    break;
                default:
                    retval = 0;
                    break;
            }

            return retval;
        }

        public Byte AppFWversionLow
        {
            get { return mAppFWversionLow; }
        }

        public Byte AppFWversionHigh
        {
            get { return mAppFWversionHigh; }
        }

        public string AppFWversion
        {
            get { return mAppFWversionHigh.ToString() + "." + mAppFWversionLow.ToString(); }
        }

        public Byte CANdeviceAddr
        {
            get { return mCANdeviceAddr; }
        }

        public UInt32 AppFWstartAddr
        {
            get { return mAppFWstartAddr; }
        }

        public UInt32 AppFWendAddr
        {
            get { return mAppFWendAddr; }
        }


    }
}
