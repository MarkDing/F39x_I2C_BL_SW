using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Windows.Forms;
using System.Diagnostics;

namespace SerialBootloaderDataSource
{
    public class HexImage : HexImageInfo
    {
        public enum FileOpenResult { ReadSuccess, ReadError, ReadCancelled };

        public const int MaxFlashSize = 128 * 1024; // 128kB maximum flash size

        //Fields
        protected bool mHexImageValid = false;
        protected byte[] mHexImage = new byte[MaxFlashSize];
        protected bool[] mPageSpecified512 = new bool[MaxFlashSize / 512];
        protected bool[] mPageSpecified1024 = new bool[MaxFlashSize /1024];
        protected string mHexFileName = "";

        //Other Fields
        private UInt32 mLowestSpecifiedAddr = 0xFFFFFFFF;
        private UInt32 mHighestSpecifiedAddr = 0;

        // Properties
        public byte[] FlattenedHexImage
        {
            get { return mHexImage; }
        }

        public string HexFileName
        {
            get { return mHexFileName; }
        }

        public bool[] PageSpecified512
        {
            get { return mPageSpecified512; }
        }

        public bool[] PageSpecified1024
        {
            get { return mPageSpecified1024; }
        }

        public bool HexImageValid
        {
            get { return mHexImageValid; }
        }

        /// <summary>
        /// Constructor
        /// </summary>
        public HexImage()
        {

        }

        /// <summary>
        /// Initializes the image array and other variables.
        /// </summary>
        public void InitHexImage()
        {
            for (int i = 0; i < MaxFlashSize; i++)
            {
                mHexImage[i] = 0xFF;
            }

            for (int i = 0; i < (MaxFlashSize/512); i++)
            {
                mPageSpecified512[i] = false;
            }

            for (int i = 0; i < (MaxFlashSize/1024); i++)
            {
                mPageSpecified1024[i] = false;
            }

            mLowestSpecifiedAddr = 0xFFFFFFFF;
            mHighestSpecifiedAddr = 0;

            mHexImageValid = false;
        }

        /// <summary>
        /// Presents a file dialog box, and then opens and reads the selected hex file. 
        /// If successful, the object's fields are appropriately updated.
        /// </summary>
        /// <returns></returns>
        public FileOpenResult OpenHexFile()
        {
            FileOpenResult retval = FileOpenResult.ReadCancelled;

            OpenFileDialog openFileDialog1 = new OpenFileDialog();

            openFileDialog1.InitialDirectory = "c:\\";
            openFileDialog1.Filter = "All files (*.*)|*.*|Hex files (*.hex)|*.hex";
            openFileDialog1.ShowReadOnly = true;
            openFileDialog1.ReadOnlyChecked = true;
            openFileDialog1.FilterIndex = 2;
            openFileDialog1.RestoreDirectory = true;

            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                InitHexImage();
                mHexImageValid = false;

                mHexFileName = openFileDialog1.FileName.ToString();

                try
                {
                    Stream HexFileStream;
                    if ((HexFileStream = openFileDialog1.OpenFile()) != null)
                    {
                        InitHexImage();
                        if (ReadHexFile(HexFileStream) == true)
                        {
                            //MessageBox.Show("Hex file read successfully!");
                            Debug.WriteLine("Hex file read successfully!");
                            //serialComm.DisplayStatus(Serial_Comm.OutputStyle.Status, "Hex file read successfully!" + Environment.NewLine);
                            mHexImageValid = true;
                            
                            // The first byte of AppFW space has to be the lowest specified addr in the hex file
                            mAppFWstartAddr = mLowestSpecifiedAddr;

                            // Add number of signature/CRC bytes to highest specified addr to get app FW end addr
//                            mAppFWendAddr = mHighestSpecifiedAddr + (int)AppFWimageInfoBlock.sigByte3 + 1; // Add 1 because enum starts at 0
                            mAppFWendAddr = mHighestSpecifiedAddr; 

                            InitHexImageInfo(ref mHexImage);

                            retval = FileOpenResult.ReadSuccess;
                        }
                        else
                        {
                            retval = FileOpenResult.ReadError;
                            //MessageBox.Show("Unknown error reading the Hex file!");
                            //serialComm.DisplayStatus(Serial_Comm.OutputStyle.Error, "Unknown error reading the Hex file!" + Environment.NewLine);
                        }
                    }
                }
                catch (Exception ex)
                {
                    //MessageBox.Show("Error: Could not read file from disk. Original error: " + ex.Message);
                    //serialComm.DisplayStatus(Serial_Comm.OutputStyle.Error, "Error: Could not read file from disk. Original error: " + ex.Message + Environment.NewLine);
                    Debug.WriteLine("Error: Could not read file from disk. Original error: " + ex.Message);
                    retval = FileOpenResult.ReadError;
                }
            }

            return retval;
        }

        /// <summary>
        /// Reads a hex file stream that has been opened by the calling method.
        /// </summary>
        /// <param name="fs"></param>
        /// <returns></returns>
        private bool ReadHexFile(Stream fs)
        {
            StreamReader r = new StreamReader(fs);
            string rawLine;

            int line_index;
            byte running_checksum;

            byte rec_len;
            byte addr_temp;
            uint addr_offset;
            byte rec_type;
            byte data_byte;
            byte line_checksum;

            bool img_parse_success = false;
            bool parse_error = false;

            while (((rawLine = r.ReadLine()) != null) && (img_parse_success == false) && (parse_error == false))
            {
                Debug.WriteLine(rawLine);

                /*
                 * Intel Hex File Line Format
                    :10246200464C5549442050524F46494C4500464C33
                    |||||||||||                              CC->Checksum
                    |||||||||DD->Data
                    |||||||TT->Record Type
                    |||AAAA->Address
                    |LL->Record Length
                    :->Colon
                 * 
                 */

                line_index = 0;
                if (rawLine.Substring(line_index++, 1) == ":")
                {
                    running_checksum = 0;

                    rec_len = byte.Parse(rawLine.Substring(line_index, 2), System.Globalization.NumberStyles.HexNumber);
                    line_index += 2;
                    running_checksum += rec_len;

                    addr_temp = byte.Parse(rawLine.Substring(line_index, 2), System.Globalization.NumberStyles.HexNumber);
                    running_checksum += addr_temp;

                    addr_temp = byte.Parse(rawLine.Substring(line_index + 2, 2), System.Globalization.NumberStyles.HexNumber);
                    running_checksum += addr_temp;

                    addr_offset = uint.Parse(rawLine.Substring(line_index, 4), System.Globalization.NumberStyles.HexNumber);
                    line_index += 4;

                    rec_type = byte.Parse(rawLine.Substring(line_index, 2), System.Globalization.NumberStyles.HexNumber);
                    line_index += 2;
                    running_checksum += rec_type;

                    switch (rec_type)
                    {
                        case 0x00:  // Data Record
                            //Loop through the data and store it in our data array that maps to memory
                            for (uint data_index = 0; data_index < rec_len; data_index++)
                            {
                                data_byte = byte.Parse(rawLine.Substring(line_index, 2), System.Globalization.NumberStyles.HexNumber);
                                line_index += 2;

                                uint addr = addr_offset + data_index;

                                if (addr >= MaxFlashSize)
                                {
                                    Debug.WriteLine("Hex file read error: Addr from hex file out of bounds");
                                    parse_error = true;
                                    break;
                                }
                                else
                                {
                                    if (addr < mLowestSpecifiedAddr)
                                    {
                                        mLowestSpecifiedAddr = addr;
                                    }
                                    if (addr > mHighestSpecifiedAddr)
                                    {
                                        mHighestSpecifiedAddr = addr;
                                    }

                                    mHexImage[addr] = data_byte;
                                    mPageSpecified512[addr >> 9] = true;
                                    mPageSpecified1024[addr >> 10] = true;
                                    running_checksum += data_byte;
                                }
                            }
                            if (parse_error)
                            {
                                break;
                            }

                            // Read the line checksum
                            line_checksum = byte.Parse(rawLine.Substring(line_index, 2), System.Globalization.NumberStyles.HexNumber);
                            running_checksum += line_checksum;

                            // If the final sum isn't 0, there was an error
                            if (running_checksum != 0x00)
                            {
                                parse_error = true;
                                break;
                            }
                            break;

                        case 0x01:  // End-of-File Record
                            img_parse_success = true;
                            break;

                        default:    // Unknown record type
                            parse_error = true;
                            break;
                    }

                    if (parse_error)
                    {
                        break;
                    }
                }
                else
                {
                    parse_error = true;
                    break;
                }
            }

            r.Close();

            if ((img_parse_success == true) && (parse_error == false))
            {
                Debug.WriteLine("Hex file read successfully!");
                return true;
            }
            else
            {
                Debug.WriteLine("Hex file read error");
                return false;
            }
        }

        /// <summary>
        /// This enum lists the bytes in order they are sent from
        /// the DataSource to the Master in response to a GetHexImageInfo request.
        /// </summary>
        public enum GetHexImageInfoRsp
        {
            ack = 0,
            responseLength,
            MCUcode,
            BLtype,
            flashPageSizeCode,
            appFWver0,
            appFWver1,
            CANdeviceAddr,
            appFWstartAddr0,
            appFWstartAddr1,
            appFWstartAddr2,
            appFWendAddr0,
            appFWendAddr1,
            appFWendAddr2,
            EndValue
        }

        public void LoadHexImageInfoResponse (ref byte[] buf)
        {
            buf[(int)GetHexImageInfoRsp.ack] = SerialCommandProcessor.SRC_RSP_OK;
            buf[(int)GetHexImageInfoRsp.responseLength] = (byte)GetHexImageInfoRsp.EndValue;
            buf[(int)GetHexImageInfoRsp.MCUcode] = MCUcode;
            buf[(int)GetHexImageInfoRsp.BLtype] = BLtypeRaw;
            buf[(int)GetHexImageInfoRsp.flashPageSizeCode] = FlashPageSizeCodeRaw;
            buf[(int)GetHexImageInfoRsp.appFWver0] = AppFWversionLow;
            buf[(int)GetHexImageInfoRsp.appFWver1] = AppFWversionHigh;
            buf[(int)GetHexImageInfoRsp.CANdeviceAddr] = CANdeviceAddr;
            buf[(int)GetHexImageInfoRsp.appFWstartAddr0] = (byte)(AppFWstartAddr & 0xFF);
            buf[(int)GetHexImageInfoRsp.appFWstartAddr1] = (byte)((AppFWstartAddr & 0xFF00) >> 8);
            buf[(int)GetHexImageInfoRsp.appFWstartAddr2] = (byte)((AppFWstartAddr & 0xFF0000) >> 16);
            buf[(int)GetHexImageInfoRsp.appFWendAddr0] = (byte)(AppFWendAddr & 0xFF);
            buf[(int)GetHexImageInfoRsp.appFWendAddr1] = (byte)((AppFWendAddr & 0xFF00) >> 8);
            buf[(int)GetHexImageInfoRsp.appFWendAddr2] = (byte)((AppFWendAddr & 0xFF0000) >> 16);
        }

        public void ExtractHexImageBytes(ref byte[] outputBuf, int outputBufOffset, uint hexImageStartAddr, int numBytes)
        {
            uint numBytesLimit = (uint)numBytes;

            if ((hexImageStartAddr + numBytes - 1) > AppFWendAddr)
            {
                numBytesLimit = AppFWendAddr - hexImageStartAddr;
            }

            for (int i = 0; i < numBytesLimit; i++)
            {
                outputBuf[outputBufOffset + i] = mHexImage[hexImageStartAddr + i];
            }
        }

        public UInt16 GetHexImagePartialCRC(int buf_offset, int numBytes)
        {
            UInt16 crc = 0;

            for (int i = 0; i < numBytes; i++)
            {
                crc = UpdateCRC(crc, mHexImage[buf_offset + i]);
            }
            return crc;
        }

        public UInt16 UpdateCRC (UInt16 crc, Byte newbyte)
        {
            const UInt16 CRCpoly = 0x8408;                 // CRC16-CCITT FCS (X^16+X^12+X^5+1)

            crc = (UInt16)((uint)crc ^ (uint)newbyte);

            for (int i = 0; i < 8; i++)
            {
              if (((uint)crc & (uint)0x0001) != 0)
              {
                 crc = (UInt16)((uint)crc >> 1);
                 crc = (UInt16)((uint)crc ^ (uint)CRCpoly);
              }
              else
              {
                 crc = (UInt16)((uint)crc >> 1);
              }
            }
            return crc;
            }
    }
}
