using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace SerialBootloaderDataSource
{
    public partial class MainForm : Form
    {
        HexImage hexImage = new HexImage();
        TargetInfo targetInfo = new TargetInfo();
        
        SerialCommandProcessor serialCommProc;
        DisplayManager displayManager;

        public MainForm()
        {
            InitializeComponent();
        }

        private void MainForm_Load(object sender, EventArgs e)
        {
            // Instantiate a new SerialCommandProcessor object and init the members
            serialCommProc = new SerialCommandProcessor(hexImage, targetInfo, OpenCOMportButton);

            // Instantiate a new DisplayManager object and init the members with the data destinations and sources.
            displayManager = new DisplayManager(EventLogRichTextBox, InfoBlockDataGridView, hexImage, serialCommProc);

            // Simple substitute for an About box:
            Version ver = System.Reflection.Assembly.GetExecutingAssembly().GetName().Version;
            displayManager.PostToTextDisplayBox(DisplayManager.OutputStyle.Status, 
                DisplayManager.OutputLevel.AlwaysShow,
                "Silicon Labs MCU Serial Bootloader DataSource v" + ver.Major.ToString() 
                + "." + ver.Minor.ToString() + "." + Environment.NewLine);
            
            // Simple substitute for a Help button:
            displayManager.PostToTextDisplayBox(DisplayManager.OutputStyle.Status,
                DisplayManager.OutputLevel.AlwaysShow,
                "Please select a Hex file and then open the COM port."
                + Environment.NewLine + Environment.NewLine);

            // Subscribe the handler in DisplayManager to the Event published
            // by SerialCommandProcessor
            serialCommProc.SubScribeUpdateDisplayEvent(
                new UpdateDisplayDelegate(displayManager.HandleUpdateDisplayEvent));

            // Make Verbose checked by default
            VerboseCheckBox.Checked = true;
            displayManager.Verbose = VerboseCheckBox.Checked;

            // Fill up the combo boxes
            ListSerialPortOptions();
        }

        private void ListSerialPortOptions()
        {
            serialCommProc.ListAvailableSerialPorts(PortComboBox);
            serialCommProc.ListBaudRateOptions(BaudRateComboBox);
            serialCommProc.ListDataBitsOptions(DataBitsComboBox);
            serialCommProc.ListParityOptions(ParityComboBox);
            serialCommProc.ListStopBitsOptions(StopBitsComboBox);
            serialCommProc.ListFlowControlOptions(FlowControlComboBox);
        }

        private void SelectHexFileButton_Click(object sender, EventArgs e)
        {
            HexImage.FileOpenResult result = hexImage.OpenHexFile();
            if (result == HexImage.FileOpenResult.ReadSuccess)
            {
                displayManager.PostToTextDisplayBox(DisplayManager.OutputStyle.Status,
                    DisplayManager.OutputLevel.AlwaysShow,
                    "Hex File was processed successfully! See table for extracted info." + Environment.NewLine + Environment.NewLine);
                
                displayManager.DisplayHexImageInfoInGrid();
                serialCommProc.FinishPriorPendingRequest(false);
            }
            else if (result == HexImage.FileOpenResult.ReadError)
            {
                displayManager.ClearHexImageInfoInGrid();

                displayManager.PostToTextDisplayBox(DisplayManager.OutputStyle.Error,
                    DisplayManager.OutputLevel.AlwaysShow,
                    "Error reading Hex File." + Environment.NewLine + Environment.NewLine);
                serialCommProc.FinishPriorPendingRequest(false);
            }
        }

        private void OpenCOMportButton_Click(object sender, EventArgs e)
        {
            if ((hexImage.HexImageValid == true) || (serialCommProc.IsPortOpen == true))
            {
                UpdateSerialPortOptions();
                serialCommProc.PortOpenClose();
            }
            else
            {
                MessageBox.Show("Please select a valid hex file before opening the COM port.", "Action Required");
            }
        }

        private void UpdateSerialPortOptions()
        {
            serialCommProc.PortName = PortComboBox.Text;
            serialCommProc.BaudRate = BaudRateComboBox.Text;
            serialCommProc.DataBits = DataBitsComboBox.Text;
            serialCommProc.ParityType = ParityComboBox.Text;
            serialCommProc.StopBits = StopBitsComboBox.Text;
            serialCommProc.FlowControlType = FlowControlComboBox.Text;
        }

        private void VerboseCheckBox_CheckedChanged(object sender, EventArgs e)
        {
            displayManager.Verbose = VerboseCheckBox.Checked;
        }

        private void ShowRawDataCheckBox_CheckedChanged(object sender, EventArgs e)
        {
            displayManager.ShowRawData = ShowRawDataCheckBox.Checked;
        }

        private void button1_Click(object sender, EventArgs e)
        {

        }

        private void ClearDisplayButton_Click(object sender, EventArgs e)
        {
            displayManager.ClearTextDisplayBox();
        }

        private void UpdateAppFWButton_Click(object sender, EventArgs e)
        {
            if (serialCommProc.ResponseToMasterPending == true)
            {
                serialCommProc.UpdateAppFW = true;
                serialCommProc.FinishPriorPendingRequest(true);
            }
            else
            {
                MessageBox.Show("Cannot initiate the bootloading process from the PC." + Environment.NewLine +
                    "Please initiate bootload from the Master MCU.", "Button Inactive At This Time");
            }
        }
    }
}
