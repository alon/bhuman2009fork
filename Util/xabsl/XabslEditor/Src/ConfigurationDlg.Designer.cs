namespace XabslEditor
{
  partial class ConfigurationDlg
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
        this.label1 = new System.Windows.Forms.Label();
        this.textBoxName = new System.Windows.Forms.TextBox();
        this.groupBox1 = new System.Windows.Forms.GroupBox();
        this.buttonDump = new System.Windows.Forms.Button();
        this.label6 = new System.Windows.Forms.Label();
        this.textBoxDump = new System.Windows.Forms.TextBox();
        this.buttonApi = new System.Windows.Forms.Button();
        this.buttonBuild = new System.Windows.Forms.Button();
        this.buttonCompiler = new System.Windows.Forms.Button();
        this.label4 = new System.Windows.Forms.Label();
        this.textBoxApi = new System.Windows.Forms.TextBox();
        this.textBoxBuild = new System.Windows.Forms.TextBox();
        this.label3 = new System.Windows.Forms.Label();
        this.textBoxCompiler = new System.Windows.Forms.TextBox();
        this.label2 = new System.Windows.Forms.Label();
        this.buttonSave = new System.Windows.Forms.Button();
        this.buttonCancel = new System.Windows.Forms.Button();
        this.openFileDialog = new System.Windows.Forms.OpenFileDialog();
        this.folderBrowserDialog = new System.Windows.Forms.FolderBrowserDialog();
        this.textBoxDot = new System.Windows.Forms.TextBox();
        this.label5 = new System.Windows.Forms.Label();
        this.buttonDot = new System.Windows.Forms.Button();
        this.groupBox1.SuspendLayout();
        this.SuspendLayout();
        // 
        // label1
        // 
        this.label1.AutoSize = true;
        this.label1.Location = new System.Drawing.Point(21, 9);
        this.label1.Name = "label1";
        this.label1.Size = new System.Drawing.Size(38, 13);
        this.label1.TabIndex = 0;
        this.label1.Text = "Name:";
        // 
        // textBoxName
        // 
        this.textBoxName.Location = new System.Drawing.Point(140, 6);
        this.textBoxName.Name = "textBoxName";
        this.textBoxName.Size = new System.Drawing.Size(272, 20);
        this.textBoxName.TabIndex = 1;
        // 
        // groupBox1
        // 
        this.groupBox1.Controls.Add(this.buttonDump);
        this.groupBox1.Controls.Add(this.label6);
        this.groupBox1.Controls.Add(this.textBoxDump);
        this.groupBox1.Controls.Add(this.buttonApi);
        this.groupBox1.Controls.Add(this.buttonBuild);
        this.groupBox1.Controls.Add(this.buttonCompiler);
        this.groupBox1.Controls.Add(this.label4);
        this.groupBox1.Controls.Add(this.textBoxApi);
        this.groupBox1.Controls.Add(this.textBoxBuild);
        this.groupBox1.Controls.Add(this.label3);
        this.groupBox1.Controls.Add(this.textBoxCompiler);
        this.groupBox1.Controls.Add(this.label2);
        this.groupBox1.Location = new System.Drawing.Point(15, 57);
        this.groupBox1.Name = "groupBox1";
        this.groupBox1.Size = new System.Drawing.Size(446, 124);
        this.groupBox1.TabIndex = 2;
        this.groupBox1.TabStop = false;
        this.groupBox1.Text = "XABSL compiler options";
        // 
        // buttonDump
        // 
        this.buttonDump.Location = new System.Drawing.Point(404, 66);
        this.buttonDump.Name = "buttonDump";
        this.buttonDump.Size = new System.Drawing.Size(28, 19);
        this.buttonDump.TabIndex = 11;
        this.buttonDump.Text = "...";
        this.buttonDump.UseVisualStyleBackColor = true;
        this.buttonDump.Click += new System.EventHandler(this.buttonBrowseFolder_Click);
        // 
        // label6
        // 
        this.label6.AutoSize = true;
        this.label6.Location = new System.Drawing.Point(6, 69);
        this.label6.Name = "label6";
        this.label6.Size = new System.Drawing.Size(47, 13);
        this.label6.TabIndex = 10;
        this.label6.Text = "dump-dir";
        // 
        // textBoxDump
        // 
        this.textBoxDump.Location = new System.Drawing.Point(125, 66);
        this.textBoxDump.Name = "textBoxDump";
        this.textBoxDump.Size = new System.Drawing.Size(272, 20);
        this.textBoxDump.TabIndex = 9;
        // 
        // buttonApi
        // 
        this.buttonApi.Location = new System.Drawing.Point(404, 93);
        this.buttonApi.Name = "buttonApi";
        this.buttonApi.Size = new System.Drawing.Size(28, 19);
        this.buttonApi.TabIndex = 8;
        this.buttonApi.Text = "...";
        this.buttonApi.UseVisualStyleBackColor = true;
        this.buttonApi.Click += new System.EventHandler(this.buttonBrowseFile_Click);
        // 
        // buttonBuild
        // 
        this.buttonBuild.Location = new System.Drawing.Point(404, 43);
        this.buttonBuild.Name = "buttonBuild";
        this.buttonBuild.Size = new System.Drawing.Size(28, 19);
        this.buttonBuild.TabIndex = 7;
        this.buttonBuild.Text = "...";
        this.buttonBuild.UseVisualStyleBackColor = true;
        this.buttonBuild.Click += new System.EventHandler(this.buttonBrowseFolder_Click);
        // 
        // buttonCompiler
        // 
        this.buttonCompiler.Location = new System.Drawing.Point(404, 18);
        this.buttonCompiler.Name = "buttonCompiler";
        this.buttonCompiler.Size = new System.Drawing.Size(28, 19);
        this.buttonCompiler.TabIndex = 6;
        this.buttonCompiler.Text = "...";
        this.buttonCompiler.UseVisualStyleBackColor = true;
        this.buttonCompiler.Click += new System.EventHandler(this.buttonBrowseFile_Click);
        // 
        // label4
        // 
        this.label4.AutoSize = true;
        this.label4.Location = new System.Drawing.Point(6, 96);
        this.label4.Name = "label4";
        this.label4.Size = new System.Drawing.Size(37, 13);
        this.label4.TabIndex = 5;
        this.label4.Text = "api-file";
        // 
        // textBoxApi
        // 
        this.textBoxApi.Location = new System.Drawing.Point(125, 92);
        this.textBoxApi.Name = "textBoxApi";
        this.textBoxApi.Size = new System.Drawing.Size(272, 20);
        this.textBoxApi.TabIndex = 4;
        // 
        // textBoxBuild
        // 
        this.textBoxBuild.Location = new System.Drawing.Point(125, 43);
        this.textBoxBuild.Name = "textBoxBuild";
        this.textBoxBuild.Size = new System.Drawing.Size(272, 20);
        this.textBoxBuild.TabIndex = 3;
        // 
        // label3
        // 
        this.label3.AutoSize = true;
        this.label3.Location = new System.Drawing.Point(6, 46);
        this.label3.Name = "label3";
        this.label3.Size = new System.Drawing.Size(46, 13);
        this.label3.TabIndex = 2;
        this.label3.Text = "build dir:";
        // 
        // textBoxCompiler
        // 
        this.textBoxCompiler.Location = new System.Drawing.Point(125, 17);
        this.textBoxCompiler.Name = "textBoxCompiler";
        this.textBoxCompiler.Size = new System.Drawing.Size(272, 20);
        this.textBoxCompiler.TabIndex = 1;
        // 
        // label2
        // 
        this.label2.AutoSize = true;
        this.label2.Location = new System.Drawing.Point(6, 20);
        this.label2.Name = "label2";
        this.label2.RightToLeft = System.Windows.Forms.RightToLeft.No;
        this.label2.Size = new System.Drawing.Size(123, 13);
        this.label2.TabIndex = 0;
        this.label2.Text = "Path to XABSL compiler:";
        // 
        // buttonSave
        // 
        this.buttonSave.DialogResult = System.Windows.Forms.DialogResult.OK;
        this.buttonSave.Location = new System.Drawing.Point(15, 188);
        this.buttonSave.Name = "buttonSave";
        this.buttonSave.Size = new System.Drawing.Size(80, 22);
        this.buttonSave.TabIndex = 3;
        this.buttonSave.Text = "&Save";
        this.buttonSave.UseVisualStyleBackColor = true;
        this.buttonSave.Click += new System.EventHandler(this.buttonSave_Click);
        // 
        // buttonCancel
        // 
        this.buttonCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
        this.buttonCancel.Location = new System.Drawing.Point(378, 187);
        this.buttonCancel.Name = "buttonCancel";
        this.buttonCancel.Size = new System.Drawing.Size(83, 23);
        this.buttonCancel.TabIndex = 4;
        this.buttonCancel.Text = "&Cancel";
        this.buttonCancel.UseVisualStyleBackColor = true;
        this.buttonCancel.Click += new System.EventHandler(this.buttonCancel_Click);
        // 
        // textBoxDot
        // 
        this.textBoxDot.Location = new System.Drawing.Point(140, 31);
        this.textBoxDot.Name = "textBoxDot";
        this.textBoxDot.Size = new System.Drawing.Size(272, 20);
        this.textBoxDot.TabIndex = 5;
        // 
        // label5
        // 
        this.label5.AutoSize = true;
        this.label5.Location = new System.Drawing.Point(21, 34);
        this.label5.Name = "label5";
        this.label5.Size = new System.Drawing.Size(62, 13);
        this.label5.TabIndex = 6;
        this.label5.Text = "Path to dot:";
        // 
        // buttonDot
        // 
        this.buttonDot.Location = new System.Drawing.Point(419, 31);
        this.buttonDot.Name = "buttonDot";
        this.buttonDot.Size = new System.Drawing.Size(28, 19);
        this.buttonDot.TabIndex = 7;
        this.buttonDot.Text = "...";
        this.buttonDot.UseVisualStyleBackColor = true;
        this.buttonDot.Click += new System.EventHandler(this.buttonBrowseFile_Click);
        // 
        // ConfigurationDlg
        // 
        this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
        this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
        this.ClientSize = new System.Drawing.Size(473, 217);
        this.ControlBox = false;
        this.Controls.Add(this.buttonDot);
        this.Controls.Add(this.label5);
        this.Controls.Add(this.textBoxDot);
        this.Controls.Add(this.buttonCancel);
        this.Controls.Add(this.buttonSave);
        this.Controls.Add(this.groupBox1);
        this.Controls.Add(this.textBoxName);
        this.Controls.Add(this.label1);
        this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
        this.Name = "ConfigurationDlg";
        this.ShowIcon = false;
        this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
        this.Text = "Configure a behavior";
        this.groupBox1.ResumeLayout(false);
        this.groupBox1.PerformLayout();
        this.ResumeLayout(false);
        this.PerformLayout();

    }

    #endregion

    private System.Windows.Forms.Label label1;
    private System.Windows.Forms.TextBox textBoxName;
    private System.Windows.Forms.GroupBox groupBox1;
    private System.Windows.Forms.Button buttonSave;
    private System.Windows.Forms.Button buttonCancel;
    private System.Windows.Forms.Label label2;
    private System.Windows.Forms.TextBox textBoxCompiler;
    private System.Windows.Forms.Label label3;
    private System.Windows.Forms.TextBox textBoxApi;
    private System.Windows.Forms.TextBox textBoxBuild;
    private System.Windows.Forms.Label label4;
    private System.Windows.Forms.Button buttonCompiler;
    private System.Windows.Forms.Button buttonApi;
    private System.Windows.Forms.Button buttonBuild;
    private System.Windows.Forms.OpenFileDialog openFileDialog;
    private System.Windows.Forms.FolderBrowserDialog folderBrowserDialog;
    private System.Windows.Forms.TextBox textBoxDot;
    private System.Windows.Forms.Label label5;
    private System.Windows.Forms.Button buttonDot;
    private System.Windows.Forms.Button buttonDump;
    private System.Windows.Forms.Label label6;
    private System.Windows.Forms.TextBox textBoxDump;
  }
}