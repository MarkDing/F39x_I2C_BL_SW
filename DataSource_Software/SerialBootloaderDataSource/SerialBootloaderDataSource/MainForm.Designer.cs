namespace SerialBootloaderDataSource
{
    partial class MainForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.ShowRawDataCheckBox = new System.Windows.Forms.CheckBox();
            this.VerboseCheckBox = new System.Windows.Forms.CheckBox();
            this.ClearDisplayButton = new System.Windows.Forms.Button();
            this.EventLogRichTextBox = new System.Windows.Forms.RichTextBox();
            this.InfoBlockDataGridView = new System.Windows.Forms.DataGridView();
            this.mFieldType = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.mHexImageInfo = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.mTargetInfo = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.SelectHexFileButton = new System.Windows.Forms.Button();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.label7 = new System.Windows.Forms.Label();
            this.label8 = new System.Windows.Forms.Label();
            this.label9 = new System.Windows.Forms.Label();
            this.label10 = new System.Windows.Forms.Label();
            this.label11 = new System.Windows.Forms.Label();
            this.label12 = new System.Windows.Forms.Label();
            this.PortComboBox = new System.Windows.Forms.ComboBox();
            this.FlowControlComboBox = new System.Windows.Forms.ComboBox();
            this.StopBitsComboBox = new System.Windows.Forms.ComboBox();
            this.ParityComboBox = new System.Windows.Forms.ComboBox();
            this.DataBitsComboBox = new System.Windows.Forms.ComboBox();
            this.BaudRateComboBox = new System.Windows.Forms.ComboBox();
            this.OpenCOMportButton = new System.Windows.Forms.Button();
            this.UpdateAppFWButton = new System.Windows.Forms.Button();
            this.groupBox2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.InfoBlockDataGridView)).BeginInit();
            this.groupBox3.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.ShowRawDataCheckBox);
            this.groupBox2.Controls.Add(this.VerboseCheckBox);
            this.groupBox2.Controls.Add(this.ClearDisplayButton);
            this.groupBox2.Controls.Add(this.EventLogRichTextBox);
            this.groupBox2.Location = new System.Drawing.Point(238, 63);
            this.groupBox2.Margin = new System.Windows.Forms.Padding(2);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Padding = new System.Windows.Forms.Padding(2);
            this.groupBox2.Size = new System.Drawing.Size(349, 372);
            this.groupBox2.TabIndex = 1;
            this.groupBox2.TabStop = false;
            // 
            // ShowRawDataCheckBox
            // 
            this.ShowRawDataCheckBox.AutoSize = true;
            this.ShowRawDataCheckBox.Location = new System.Drawing.Point(71, 345);
            this.ShowRawDataCheckBox.Margin = new System.Windows.Forms.Padding(2);
            this.ShowRawDataCheckBox.Name = "ShowRawDataCheckBox";
            this.ShowRawDataCheckBox.Size = new System.Drawing.Size(104, 17);
            this.ShowRawDataCheckBox.TabIndex = 3;
            this.ShowRawDataCheckBox.Text = "Show Raw Data";
            this.ShowRawDataCheckBox.UseVisualStyleBackColor = true;
            this.ShowRawDataCheckBox.CheckedChanged += new System.EventHandler(this.ShowRawDataCheckBox_CheckedChanged);
            // 
            // VerboseCheckBox
            // 
            this.VerboseCheckBox.AutoSize = true;
            this.VerboseCheckBox.Location = new System.Drawing.Point(4, 345);
            this.VerboseCheckBox.Margin = new System.Windows.Forms.Padding(2);
            this.VerboseCheckBox.Name = "VerboseCheckBox";
            this.VerboseCheckBox.Size = new System.Drawing.Size(65, 17);
            this.VerboseCheckBox.TabIndex = 2;
            this.VerboseCheckBox.Text = "Verbose";
            this.VerboseCheckBox.UseVisualStyleBackColor = true;
            this.VerboseCheckBox.CheckedChanged += new System.EventHandler(this.VerboseCheckBox_CheckedChanged);
            // 
            // ClearDisplayButton
            // 
            this.ClearDisplayButton.Location = new System.Drawing.Point(267, 341);
            this.ClearDisplayButton.Margin = new System.Windows.Forms.Padding(2);
            this.ClearDisplayButton.Name = "ClearDisplayButton";
            this.ClearDisplayButton.Size = new System.Drawing.Size(77, 24);
            this.ClearDisplayButton.TabIndex = 1;
            this.ClearDisplayButton.Text = "Clear Display";
            this.ClearDisplayButton.UseVisualStyleBackColor = true;
            this.ClearDisplayButton.Click += new System.EventHandler(this.ClearDisplayButton_Click);
            // 
            // EventLogRichTextBox
            // 
            this.EventLogRichTextBox.Location = new System.Drawing.Point(4, 16);
            this.EventLogRichTextBox.Margin = new System.Windows.Forms.Padding(2);
            this.EventLogRichTextBox.Name = "EventLogRichTextBox";
            this.EventLogRichTextBox.Size = new System.Drawing.Size(341, 321);
            this.EventLogRichTextBox.TabIndex = 0;
            this.EventLogRichTextBox.Text = "";
            // 
            // InfoBlockDataGridView
            // 
            this.InfoBlockDataGridView.AllowUserToAddRows = false;
            this.InfoBlockDataGridView.AllowUserToDeleteRows = false;
            this.InfoBlockDataGridView.AllowUserToResizeColumns = false;
            this.InfoBlockDataGridView.AllowUserToResizeRows = false;
            this.InfoBlockDataGridView.BackgroundColor = System.Drawing.SystemColors.ControlLight;
            this.InfoBlockDataGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.InfoBlockDataGridView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.mFieldType,
            this.mHexImageInfo,
            this.mTargetInfo});
            this.InfoBlockDataGridView.Enabled = false;
            this.InfoBlockDataGridView.Location = new System.Drawing.Point(8, 69);
            this.InfoBlockDataGridView.Margin = new System.Windows.Forms.Padding(2);
            this.InfoBlockDataGridView.MultiSelect = false;
            this.InfoBlockDataGridView.Name = "InfoBlockDataGridView";
            this.InfoBlockDataGridView.ReadOnly = true;
            this.InfoBlockDataGridView.RowHeadersVisible = false;
            this.InfoBlockDataGridView.RowTemplate.Height = 24;
            this.InfoBlockDataGridView.ScrollBars = System.Windows.Forms.ScrollBars.None;
            this.InfoBlockDataGridView.Size = new System.Drawing.Size(225, 331);
            this.InfoBlockDataGridView.TabIndex = 2;
            this.InfoBlockDataGridView.TabStop = false;
            // 
            // mFieldType
            // 
            this.mFieldType.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.AllCellsExceptHeader;
            this.mFieldType.HeaderText = "Field Type";
            this.mFieldType.Name = "mFieldType";
            this.mFieldType.ReadOnly = true;
            this.mFieldType.Width = 5;
            // 
            // mHexImageInfo
            // 
            this.mHexImageInfo.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.AllCellsExceptHeader;
            this.mHexImageInfo.HeaderText = "Hex Image Info";
            this.mHexImageInfo.Name = "mHexImageInfo";
            this.mHexImageInfo.ReadOnly = true;
            this.mHexImageInfo.Width = 5;
            // 
            // mTargetInfo
            // 
            this.mTargetInfo.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
            this.mTargetInfo.HeaderText = "Target Info";
            this.mTargetInfo.Name = "mTargetInfo";
            this.mTargetInfo.ReadOnly = true;
            // 
            // SelectHexFileButton
            // 
            this.SelectHexFileButton.Location = new System.Drawing.Point(9, 12);
            this.SelectHexFileButton.Margin = new System.Windows.Forms.Padding(2);
            this.SelectHexFileButton.Name = "SelectHexFileButton";
            this.SelectHexFileButton.Size = new System.Drawing.Size(80, 46);
            this.SelectHexFileButton.TabIndex = 0;
            this.SelectHexFileButton.Text = "Select Hex File(s)";
            this.SelectHexFileButton.UseVisualStyleBackColor = true;
            this.SelectHexFileButton.Click += new System.EventHandler(this.SelectHexFileButton_Click);
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.label7);
            this.groupBox3.Controls.Add(this.label8);
            this.groupBox3.Controls.Add(this.label9);
            this.groupBox3.Controls.Add(this.label10);
            this.groupBox3.Controls.Add(this.label11);
            this.groupBox3.Controls.Add(this.label12);
            this.groupBox3.Controls.Add(this.PortComboBox);
            this.groupBox3.Controls.Add(this.FlowControlComboBox);
            this.groupBox3.Controls.Add(this.StopBitsComboBox);
            this.groupBox3.Controls.Add(this.ParityComboBox);
            this.groupBox3.Controls.Add(this.DataBitsComboBox);
            this.groupBox3.Controls.Add(this.BaudRateComboBox);
            this.groupBox3.Controls.Add(this.OpenCOMportButton);
            this.groupBox3.Location = new System.Drawing.Point(94, 6);
            this.groupBox3.Margin = new System.Windows.Forms.Padding(2);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Padding = new System.Windows.Forms.Padding(2);
            this.groupBox3.Size = new System.Drawing.Size(493, 53);
            this.groupBox3.TabIndex = 0;
            this.groupBox3.TabStop = false;
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(327, 9);
            this.label7.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(65, 13);
            this.label7.TabIndex = 4;
            this.label7.Text = "Flow Control";
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(272, 9);
            this.label8.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(49, 13);
            this.label8.TabIndex = 4;
            this.label8.Text = "Stop Bits";
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(220, 10);
            this.label9.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(33, 13);
            this.label9.TabIndex = 4;
            this.label9.Text = "Parity";
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(155, 9);
            this.label10.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(50, 13);
            this.label10.TabIndex = 4;
            this.label10.Text = "Data Bits";
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(12, 10);
            this.label11.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(53, 13);
            this.label11.TabIndex = 4;
            this.label11.Text = "COM Port";
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Location = new System.Drawing.Point(83, 9);
            this.label12.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(58, 13);
            this.label12.TabIndex = 4;
            this.label12.Text = "Baud Rate";
            // 
            // PortComboBox
            // 
            this.PortComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.PortComboBox.FormattingEnabled = true;
            this.PortComboBox.Location = new System.Drawing.Point(7, 25);
            this.PortComboBox.Margin = new System.Windows.Forms.Padding(2);
            this.PortComboBox.Name = "PortComboBox";
            this.PortComboBox.Size = new System.Drawing.Size(65, 21);
            this.PortComboBox.TabIndex = 3;
            // 
            // FlowControlComboBox
            // 
            this.FlowControlComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.FlowControlComboBox.FormattingEnabled = true;
            this.FlowControlComboBox.Location = new System.Drawing.Point(327, 25);
            this.FlowControlComboBox.Margin = new System.Windows.Forms.Padding(2);
            this.FlowControlComboBox.Name = "FlowControlComboBox";
            this.FlowControlComboBox.Size = new System.Drawing.Size(65, 21);
            this.FlowControlComboBox.TabIndex = 3;
            // 
            // StopBitsComboBox
            // 
            this.StopBitsComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.StopBitsComboBox.FormattingEnabled = true;
            this.StopBitsComboBox.Location = new System.Drawing.Point(269, 25);
            this.StopBitsComboBox.Margin = new System.Windows.Forms.Padding(2);
            this.StopBitsComboBox.Name = "StopBitsComboBox";
            this.StopBitsComboBox.Size = new System.Drawing.Size(54, 21);
            this.StopBitsComboBox.TabIndex = 3;
            // 
            // ParityComboBox
            // 
            this.ParityComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.ParityComboBox.FormattingEnabled = true;
            this.ParityComboBox.Location = new System.Drawing.Point(211, 25);
            this.ParityComboBox.Margin = new System.Windows.Forms.Padding(2);
            this.ParityComboBox.Name = "ParityComboBox";
            this.ParityComboBox.Size = new System.Drawing.Size(55, 21);
            this.ParityComboBox.TabIndex = 3;
            // 
            // DataBitsComboBox
            // 
            this.DataBitsComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.DataBitsComboBox.FormattingEnabled = true;
            this.DataBitsComboBox.Location = new System.Drawing.Point(153, 25);
            this.DataBitsComboBox.Margin = new System.Windows.Forms.Padding(2);
            this.DataBitsComboBox.Name = "DataBitsComboBox";
            this.DataBitsComboBox.Size = new System.Drawing.Size(54, 21);
            this.DataBitsComboBox.TabIndex = 3;
            // 
            // BaudRateComboBox
            // 
            this.BaudRateComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.BaudRateComboBox.FormattingEnabled = true;
            this.BaudRateComboBox.Location = new System.Drawing.Point(75, 25);
            this.BaudRateComboBox.Margin = new System.Windows.Forms.Padding(2);
            this.BaudRateComboBox.Name = "BaudRateComboBox";
            this.BaudRateComboBox.Size = new System.Drawing.Size(74, 21);
            this.BaudRateComboBox.TabIndex = 2;
            // 
            // OpenCOMportButton
            // 
            this.OpenCOMportButton.Location = new System.Drawing.Point(398, 6);
            this.OpenCOMportButton.Margin = new System.Windows.Forms.Padding(2);
            this.OpenCOMportButton.Name = "OpenCOMportButton";
            this.OpenCOMportButton.Size = new System.Drawing.Size(94, 46);
            this.OpenCOMportButton.TabIndex = 1;
            this.OpenCOMportButton.Text = "Open COM Port";
            this.OpenCOMportButton.UseVisualStyleBackColor = true;
            this.OpenCOMportButton.Click += new System.EventHandler(this.OpenCOMportButton_Click);
            // 
            // UpdateAppFWButton
            // 
            this.UpdateAppFWButton.Location = new System.Drawing.Point(27, 410);
            this.UpdateAppFWButton.Name = "UpdateAppFWButton";
            this.UpdateAppFWButton.Size = new System.Drawing.Size(181, 25);
            this.UpdateAppFWButton.TabIndex = 5;
            this.UpdateAppFWButton.Text = "Update Application Firmware";
            this.UpdateAppFWButton.UseVisualStyleBackColor = true;
            this.UpdateAppFWButton.Click += new System.EventHandler(this.UpdateAppFWButton_Click);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(596, 446);
            this.Controls.Add(this.UpdateAppFWButton);
            this.Controls.Add(this.SelectHexFileButton);
            this.Controls.Add(this.InfoBlockDataGridView);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox3);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Margin = new System.Windows.Forms.Padding(2);
            this.MaximizeBox = false;
            this.Name = "MainForm";
            this.Text = "Silicon Labs MCU Serial Bootloader Data Source";
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.InfoBlockDataGridView)).EndInit();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.DataGridView InfoBlockDataGridView;
        private System.Windows.Forms.CheckBox ShowRawDataCheckBox;
        private System.Windows.Forms.CheckBox VerboseCheckBox;
        private System.Windows.Forms.Button ClearDisplayButton;
        private System.Windows.Forms.RichTextBox EventLogRichTextBox;
        private System.Windows.Forms.Button SelectHexFileButton;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.ComboBox PortComboBox;
        private System.Windows.Forms.ComboBox FlowControlComboBox;
        private System.Windows.Forms.ComboBox StopBitsComboBox;
        private System.Windows.Forms.ComboBox ParityComboBox;
        private System.Windows.Forms.ComboBox DataBitsComboBox;
        private System.Windows.Forms.ComboBox BaudRateComboBox;
        private System.Windows.Forms.Button OpenCOMportButton;
        private System.Windows.Forms.DataGridViewTextBoxColumn mFieldType;
        private System.Windows.Forms.DataGridViewTextBoxColumn mHexImageInfo;
        private System.Windows.Forms.DataGridViewTextBoxColumn mTargetInfo;
        private System.Windows.Forms.Button UpdateAppFWButton;

    }
}

