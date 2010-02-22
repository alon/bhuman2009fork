using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Reflection;
using System.Web;


namespace XabslEditor
{
	/// <summary>
	/// Summary description for ExceptionDialog.
	/// </summary>
	public class ExceptionDialog : System.Windows.Forms.Form
	{
    
    System.Windows.Forms.TabControl tabControl1;
    System.Windows.Forms.TextBox textBoxStackTrace;
    System.Windows.Forms.TabPage tabPage1;
    System.Windows.Forms.TabPage tabPage2;
    System.Windows.Forms.TextBox textBoxSysInfo;
    System.Windows.Forms.PictureBox pictureBox1;
    System.Windows.Forms.Button buttonAbort;
    System.Windows.Forms.Button buttonIgnore;
    System.Windows.Forms.Button buttonSubmit;
    System.Windows.Forms.Label labelMessage;
    System.Windows.Forms.LinkLabel linkLabelHelp;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System.ComponentModel.Container components = null;

    Exception  exception;

    public ExceptionDialog(Exception exception, DialogResult defaultAction)
		{
			// Required for Windows Form Designer support
			//
			InitializeComponent();

      pictureBox1.Image = SystemIcons.Warning.ToBitmap();      
      this.Icon = SystemIcons.Warning;
      this.exception = exception;
      this.Text = exception.GetType().FullName;

      switch (defaultAction) {
        case DialogResult.Abort:
          this.AcceptButton = buttonAbort;
          break;
        case DialogResult.Ignore:
          this.AcceptButton = buttonIgnore;
          break;
        default:
          throw new Exception("Invalid default action");
      }

      labelMessage.Text = exception.Message;
      if (exception.HelpLink != null) {
        linkLabelHelp.Visible = true;
        linkLabelHelp.Links.Add(
          0, linkLabelHelp.Text.Length, exception.HelpLink
        );
      }
      textBoxStackTrace.Text = exception.ToString();
      textBoxSysInfo.Text += Helpers.getSystemInfo();

#if DEBUG
#pragma warning disable 0618 // Want to use deprecated AppDomain.GetCurrentThreadID because this ID is shown in VS
      System.Console.Out.WriteLine("Exception in thread " + AppDomain.GetCurrentThreadId().ToString());
#pragma warning restore 0618
      System.Console.Out.WriteLine(textBoxStackTrace.Text);
#endif
    }

    public ExceptionDialog(Exception exception) 
      : this(exception, DialogResult.Abort)
    {
    }

    /// <summary>
    /// Show error exception (default action is abort).
    /// </summary>
    /// <param name="ex">Exception to show</param>
    /// <returns>The users's choice</returns>
    static public DialogResult ShowError(Exception ex)
    {
      Form dialog = new ExceptionDialog(ex, DialogResult.Abort);
      return  dialog.ShowDialog();
    }

    /// <summary>
    /// Show warning exception (default action is ignore).
    /// </summary>
    /// <param name="ex">Exception to show</param>
    /// <returns>The users's choice</returns>
    static public DialogResult ShowWarning(Exception ex)
    {
      Form dialog = new ExceptionDialog(ex, DialogResult.Ignore);
      return  dialog.ShowDialog();
    }

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if(components != null)
				{
					components.Dispose();
				}
			}
      base.Dispose( disposing );
		}

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent()
		{
      this.textBoxStackTrace = new System.Windows.Forms.TextBox();
      this.tabControl1 = new System.Windows.Forms.TabControl();
      this.tabPage1 = new System.Windows.Forms.TabPage();
      this.tabPage2 = new System.Windows.Forms.TabPage();
      this.textBoxSysInfo = new System.Windows.Forms.TextBox();
      this.pictureBox1 = new System.Windows.Forms.PictureBox();
      this.buttonAbort = new System.Windows.Forms.Button();
      this.buttonIgnore = new System.Windows.Forms.Button();
      this.buttonSubmit = new System.Windows.Forms.Button();
      this.labelMessage = new System.Windows.Forms.Label();
      this.linkLabelHelp = new System.Windows.Forms.LinkLabel();
      this.tabControl1.SuspendLayout();
      this.tabPage1.SuspendLayout();
      this.tabPage2.SuspendLayout();
      this.SuspendLayout();
      // 
      // textBoxStackTrace
      // 
      this.textBoxStackTrace.Dock = System.Windows.Forms.DockStyle.Fill;
      this.textBoxStackTrace.Location = new System.Drawing.Point(0, 0);
      this.textBoxStackTrace.Multiline = true;
      this.textBoxStackTrace.Name = "textBoxStackTrace";
      this.textBoxStackTrace.ReadOnly = true;
      this.textBoxStackTrace.ScrollBars = System.Windows.Forms.ScrollBars.Both;
      this.textBoxStackTrace.Size = new System.Drawing.Size(368, 134);
      this.textBoxStackTrace.TabIndex = 0;
      this.textBoxStackTrace.Text = "";
      this.textBoxStackTrace.WordWrap = false;
      // 
      // tabControl1
      // 
      this.tabControl1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
        | System.Windows.Forms.AnchorStyles.Left) 
        | System.Windows.Forms.AnchorStyles.Right)));
      this.tabControl1.Controls.Add(this.tabPage1);
      this.tabControl1.Controls.Add(this.tabPage2);
      this.tabControl1.Location = new System.Drawing.Point(8, 56);
      this.tabControl1.Name = "tabControl1";
      this.tabControl1.SelectedIndex = 0;
      this.tabControl1.Size = new System.Drawing.Size(376, 160);
      this.tabControl1.TabIndex = 1;
      // 
      // tabPage1
      // 
      this.tabPage1.Controls.Add(this.textBoxStackTrace);
      this.tabPage1.Location = new System.Drawing.Point(4, 22);
      this.tabPage1.Name = "tabPage1";
      this.tabPage1.Size = new System.Drawing.Size(368, 134);
      this.tabPage1.TabIndex = 0;
      this.tabPage1.Text = "Stack Trace";
      // 
      // tabPage2
      // 
      this.tabPage2.Controls.Add(this.textBoxSysInfo);
      this.tabPage2.Location = new System.Drawing.Point(4, 22);
      this.tabPage2.Name = "tabPage2";
      this.tabPage2.Size = new System.Drawing.Size(368, 134);
      this.tabPage2.TabIndex = 1;
      this.tabPage2.Text = "System Information";
      // 
      // textBoxSysInfo
      // 
      this.textBoxSysInfo.Dock = System.Windows.Forms.DockStyle.Fill;
      this.textBoxSysInfo.Location = new System.Drawing.Point(0, 0);
      this.textBoxSysInfo.Multiline = true;
      this.textBoxSysInfo.Name = "textBoxSysInfo";
      this.textBoxSysInfo.ReadOnly = true;
      this.textBoxSysInfo.ScrollBars = System.Windows.Forms.ScrollBars.Both;
      this.textBoxSysInfo.Size = new System.Drawing.Size(368, 134);
      this.textBoxSysInfo.TabIndex = 0;
      this.textBoxSysInfo.Text = "";
      this.textBoxSysInfo.WordWrap = false;
      // 
      // pictureBox1
      // 
      this.pictureBox1.Location = new System.Drawing.Point(8, 8);
      this.pictureBox1.Name = "pictureBox1";
      this.pictureBox1.Size = new System.Drawing.Size(32, 32);
      this.pictureBox1.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
      this.pictureBox1.TabIndex = 2;
      this.pictureBox1.TabStop = false;
      // 
      // buttonAbort
      // 
      this.buttonAbort.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
      this.buttonAbort.DialogResult = System.Windows.Forms.DialogResult.Abort;
      this.buttonAbort.Location = new System.Drawing.Point(296, 224);
      this.buttonAbort.Name = "buttonAbort";
      this.buttonAbort.Size = new System.Drawing.Size(72, 23);
      this.buttonAbort.TabIndex = 5;
      this.buttonAbort.Text = "&Abort";
      // 
      // buttonIgnore
      // 
      this.buttonIgnore.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
      this.buttonIgnore.DialogResult = System.Windows.Forms.DialogResult.Ignore;
      this.buttonIgnore.Location = new System.Drawing.Point(216, 224);
      this.buttonIgnore.Name = "buttonIgnore";
      this.buttonIgnore.Size = new System.Drawing.Size(72, 23);
      this.buttonIgnore.TabIndex = 4;
      this.buttonIgnore.Text = "&Ignore";
      // 
      // buttonSubmit
      // 
      this.buttonSubmit.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
      this.buttonSubmit.Location = new System.Drawing.Point(8, 224);
      this.buttonSubmit.Name = "buttonSubmit";
      this.buttonSubmit.Size = new System.Drawing.Size(128, 23);
      this.buttonSubmit.TabIndex = 3;
      this.buttonSubmit.Text = "&Submit Bug Report";
      this.buttonSubmit.Click += new System.EventHandler(this.buttonSubmit_Click);
      // 
      // labelMessage
      // 
      this.labelMessage.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
        | System.Windows.Forms.AnchorStyles.Right)));
      this.labelMessage.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
      this.labelMessage.Location = new System.Drawing.Point(56, 8);
      this.labelMessage.Name = "labelMessage";
      this.labelMessage.Size = new System.Drawing.Size(328, 40);
      this.labelMessage.TabIndex = 6;
      this.labelMessage.Text = "Message";
      // 
      // linkLabelHelp
      // 
      this.linkLabelHelp.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.linkLabelHelp.Location = new System.Drawing.Point(328, 56);
      this.linkLabelHelp.Name = "linkLabelHelp";
      this.linkLabelHelp.Size = new System.Drawing.Size(56, 16);
      this.linkLabelHelp.TabIndex = 2;
      this.linkLabelHelp.TabStop = true;
      this.linkLabelHelp.Text = "Get Help";
      this.linkLabelHelp.TextAlign = System.Drawing.ContentAlignment.TopRight;
      this.linkLabelHelp.Visible = false;
      this.linkLabelHelp.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.linkLabelHelp_LinkClicked);
      // 
      // ExceptionDialog
      // 
      this.AcceptButton = this.buttonAbort;
      this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
      this.CancelButton = this.buttonIgnore;
      this.ClientSize = new System.Drawing.Size(392, 254);
      this.Controls.Add(this.linkLabelHelp);
      this.Controls.Add(this.labelMessage);
      this.Controls.Add(this.buttonSubmit);
      this.Controls.Add(this.buttonIgnore);
      this.Controls.Add(this.buttonAbort);
      this.Controls.Add(this.pictureBox1);
      this.Controls.Add(this.tabControl1);
      this.MinimumSize = new System.Drawing.Size(400, 288);
      this.Name = "ExceptionDialog";
      this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
      this.Text = "Exception";
      this.tabControl1.ResumeLayout(false);
      this.tabPage1.ResumeLayout(false);
      this.tabPage2.ResumeLayout(false);
      this.ResumeLayout(false);
    }
		#endregion

    void linkLabelHelp_LinkClicked(object sender, System.Windows.Forms.LinkLabelLinkClickedEventArgs e)
    {
      System.Diagnostics.Process.Start(e.Link.LinkData.ToString());
    }


    void buttonSubmit_Click(object sender, System.EventArgs e)
    {
      string link = String.Format(
        "mailto:krause@informatik.hu-berlin.de?subject={0}&body={1}",
        HttpUtility.UrlEncode("[XablsEditorBUG] " + exception.GetType().FullName),
        HttpUtility.UrlEncode(
          "[Fill in detailed problem description here]\r\n\r\n" 
          + Helpers.getSystemInfo() + "\r\n" 
          + exception.ToString()
        )
      ).Replace("+", "%20");
      try {
        System.Diagnostics.Process.Start(link);
      } 
      catch (Exception) {
        // arrrgh.
      }
    }

	}
}
