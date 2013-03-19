using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace SerialBootloaderDataSource
{
    /// <summary>
    /// This class manages the Hex File Image
    /// of the Application FW.
    /// </summary>
    public class HexImageInfo : InfoBlock
    {
        /// <summary>
        /// This enum lists the bytes in order they are sent from
        /// the Target to the Master in response to a GetTargetInfo request.
        /// Note that the enum values are negative offsets from
        /// the last byte of AppFW space.
        /// </summary>
        public enum AppFWimageInfoBlock
        {
            sigByte0 = 0,
            sigByte1,
            sigByte2,
            sigByte3,
            infoBlockLength,
            MCUcode,
            BLtype,
            flashPageSizeCode,
            appFWver0,
            appFWver1,
            EndValue
        }

        public enum AppFWimageInfoBlockCAN
        {
            CANdeviceAddr = AppFWimageInfoBlock.EndValue
        }

        /// <summary>
        /// This method takes the hex image and initializes the fields
        /// of this class object. The hex image should have the values
        /// arranged in the order specified by the enum "AppFWimageInfoBlock".
        /// Call this only *after* setting up AppFWendAddr correctly.
        /// </summary>
        /// <param name="hexImage"></param>
        public void InitHexImageInfo(ref byte[] hexImage)
        {
            mInfoBlockLength = hexImage[mAppFWendAddr - (int)AppFWimageInfoBlock.infoBlockLength];
            mMCUcode = hexImage[mAppFWendAddr - (int)AppFWimageInfoBlock.MCUcode];
            mBLtype = hexImage[mAppFWendAddr - (int)AppFWimageInfoBlock.BLtype];
            mFlashPageSizeCode = hexImage[mAppFWendAddr - (int)AppFWimageInfoBlock.flashPageSizeCode];
            mAppFWversionLow = hexImage[mAppFWendAddr - (int)AppFWimageInfoBlock.appFWver0];
            mAppFWversionHigh = hexImage[mAppFWendAddr - (int)AppFWimageInfoBlock.appFWver1];

            mCANdeviceAddr = hexImage[mAppFWendAddr - (int)AppFWimageInfoBlockCAN.CANdeviceAddr];
        }

    }
}
