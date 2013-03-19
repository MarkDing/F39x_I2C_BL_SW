using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace SerialBootloaderDataSource
{
    /// <summary>
    /// This class manages the Target Info
    /// details received via the Master MCU.
    /// </summary>
    public class TargetInfo : InfoBlock
    {
        protected Byte mBLfwVersionLow = 0;
        protected Byte mBLfwVersionHigh = 0;
        protected Byte mBLbufferSizeCode = 0;
        protected Byte mCRCtype = 0;
        protected Byte mEncryptionType = 0;

        /// <summary>
        /// Constructor
        /// </summary>
        public TargetInfo()
        {
        }

        public string BLfwVersion
        {
            get { return mBLfwVersionHigh.ToString() + "." + mBLfwVersionLow.ToString(); }
        }

        public UInt16 BLbufferSize
        {
            get { return DecodeBLbufferSizeCode(); }
        }

        UInt16 DecodeBLbufferSizeCode()
        {
            UInt16 retval;

            switch (mBLbufferSizeCode)
            {
                case 0xFD:
                    retval = 1024;
                    break;
                case 0xFC:
                    retval = 512;
                    break;
                case 0xFB:
                    retval = 256;
                    break;
                default:
                    if (mBLbufferSizeCode < 0xFB)
                    {
                        // 0x00 thru 0xFA are actual values
                        retval = (UInt16)mBLbufferSizeCode;
                    }
                    else
                    {
                        // 0xFF, 0xFE are reserved for future expansion, if necessary.
                        retval = 0;
                    }
                    break;
            }

            return retval;
        }

        public Byte CRCtypeRaw
        {
            get { return mCRCtype; }
        }

        public string CRCtype
        {
            get { return DecodeCRCtype(); }
        }

        string DecodeCRCtype()
        {
            string retval;

            switch (mCRCtype)
            {
                case 0:
                    retval = "None";
                    break;
                case 1:
                    retval = "CRC-16-CCITT";
                    break;
                case 2:
                    retval = "CRC-32-IEEE 802.3";
                    break;
                default:
                    retval = "Invalid";
                    break;
            }

            return retval;
        }

        public Byte EncryptionTypeRaw
        {
            get { return mEncryptionType; }
        }

        public string EncryptionType
        {
            get { return DecodeEncryptionType(); }
        }

        string DecodeEncryptionType()
        {
            string retval;

            switch (mEncryptionType)
            {
                case 0:
                    retval = "None";
                    break;
                default:
                    retval = "Invalid";
                    break;
            }

            return retval;
        }

        /// <summary>
        /// This enum lists the bytes in order they are sent from
        /// the Target to the Master in response to a GetTargetInfo request.
        /// </summary>
        public enum GetTargetInfoRsp
        {
            infoLength = 0,
            BLfwVer0,
            BLfwVer1,
            MCUcode,
            BLtype,
            flashPageSizeCode,
            BLbufferSizeCode,
            CRCtype,
            encryptionType,
            appFWstartAddr0,
            appFWstartAddr1,
            appFWstartAddr2,
            appFWendAddr0,
            appFWendAddr1,
            appFWendAddr2,
            CANdeviceAddr,
            appFWver0,
            appFWver1,
            EndValue
        }

        /// <summary>
        /// This method takes a byte array and initializes the fields
        /// of this class object. The byte array should have the values
        /// arranged in the order specified by the enum "GetTargetInfoRsp".
        /// </summary>
        /// <param name="targetInfoResponse"></param>
        public void InitTargetInfo(byte[] targetInfoResponse, int responseStartOffset)
        {
            int c = responseStartOffset;

            mInfoBlockLength = targetInfoResponse[(int)GetTargetInfoRsp.infoLength + c];
            mBLfwVersionLow = targetInfoResponse[(int)GetTargetInfoRsp.BLfwVer0 + c];
            mBLfwVersionHigh = targetInfoResponse[(int)GetTargetInfoRsp.BLfwVer1 + c];
            mMCUcode = targetInfoResponse[(int)GetTargetInfoRsp.MCUcode + c];
            mBLtype = targetInfoResponse[(int)GetTargetInfoRsp.BLtype + c];
            mFlashPageSizeCode = targetInfoResponse[(int)GetTargetInfoRsp.flashPageSizeCode + c];
            mBLbufferSizeCode = targetInfoResponse[(int)GetTargetInfoRsp.BLbufferSizeCode + c];
            mCRCtype = targetInfoResponse[(int)GetTargetInfoRsp.CRCtype + c];
            mEncryptionType = targetInfoResponse[(int)GetTargetInfoRsp.encryptionType + c];
            mAppFWstartAddr = (uint)(
                                (targetInfoResponse[(int)GetTargetInfoRsp.appFWstartAddr0 + c]) |
                                (targetInfoResponse[(int)GetTargetInfoRsp.appFWstartAddr1 + c] << 8) |
                                (targetInfoResponse[(int)GetTargetInfoRsp.appFWstartAddr2 + c] << 16)
                                );
            mAppFWendAddr = (uint)(
                                (targetInfoResponse[(int)GetTargetInfoRsp.appFWendAddr0 + c]) |
                                (targetInfoResponse[(int)GetTargetInfoRsp.appFWendAddr1 + c] << 8) |
                                (targetInfoResponse[(int)GetTargetInfoRsp.appFWendAddr2 + c] << 16)
                                );
            mCANdeviceAddr = targetInfoResponse[(int)GetTargetInfoRsp.CANdeviceAddr + c];
            mAppFWversionLow = targetInfoResponse[(int)GetTargetInfoRsp.appFWver0 + c];
            mAppFWversionHigh = targetInfoResponse[(int)GetTargetInfoRsp.appFWver1 + c];         
        }

    }
}
