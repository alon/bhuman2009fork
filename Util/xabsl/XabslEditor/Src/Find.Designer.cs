namespace XabslEditor
{
  partial class Find
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
      this.textBoxFind = new System.Windows.Forms.TextBox();
      this.buttonNext = new System.Windows.Forms.Button();
      this.buttonPrev = new System.Windows.Forms.Button();
      this.buttonClose = new System.Windows.Forms.Button();
      this.checkBoxCase = new System.Windows.Forms.CheckBox();
      this.SuspendLayout();
      // 
      // label1
      // 
      this.label1.AutoSize = true;
      this.label1.Location = new System.Drawing.Point(3, 9);
      this.label1.Name = "label1";
      this.label1.Size = new System.Drawing.Size(30, 13);
      this.label1.TabIndex = 0;
      this.label1.Text = "Find:";
      // 
      // textBoxFind
      // 
      this.textBoxFind.Location = new System.Drawing.Point(39, 6);
      this.textBoxFind.Name = "textBoxFind";
      this.textBoxFind.Size = new System.Drawing.Size(204, 20);
      this.textBoxFind.TabIndex = 1;
      this.textBoxFind.TextChanged += new System.EventHandler(this.textBoxFind_TextChanged);
      // 
      // buttonNext
      // 
      this.buttonNext.Location = new System.Drawing.Point(6, 55);
      this.buttonNext.Name = "buttonNext";
      this.buttonNext.Size = new System.Drawing.Size(75, 23);
      this.buttonNext.TabIndex = 3;
      this.buttonNext.Text = "&Next";
      this.buttonNext.UseVisualStyleBackColor = true;
      this.buttonNext.Click += new System.EventHandler(this.buttonNext_Click);
      // 
      // buttonPrev
      // 
      this.buttonPrev.Location = new System.Drawing.Point(87, 55);
      this.buttonPrev.Name = "buttonPrev";
      this.buttonPrev.Size = new System.Drawing.Size(75, 23);
      this.buttonPrev.TabIndex = 4;
      this.buttonPrev.Text = "&Previous";
      this.buttonPrev.UseVisualStyleBackColor = true;
      this.buttonPrev.Click += new System.EventHandler(this.buttonPrev_Click);
      // 
      // buttonClose
      // 
      this.buttonClose.DialogResult = System.Windows.Forms.DialogResult.Cancel;
      this.buttonClose.Location = new System.Drawing.Point(168, 55);
      this.buttonClose.Name = "buttonClose";
      this.buttonClose.Size = new System.Drawing.Size(75, 23);
      this.buttonClose.TabIndex = 5;
      this.buttonClose.Text = "&Close";
      this.buttonClose.UseVisualStyleBackColor = true;
      this.buttonClose.Click += new System.EventHandler(this.buttonClose_Click);
      // 
      // checkBoxCase
      // 
      this.checkBoxCase.AutoSize = true;
      this.checkBoxCase.Location = new System.Drawing.Point(39, 32);
      this.checkBoxCase.Name = "checkBoxCase";
      this.checkBoxCase.Size = new System.Drawing.Size(93, 17);
      this.checkBoxCase.TabIndex = 2;
      this.checkBoxCase.Text = "case &sensitive";
      this.checkBoxCase.UseVisualStyleBackColor = true;
      // 
      // Find
      // 
      this.AcceptButton = this.buttonNext;
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.CancelButton = this.buttonClose;
      this.ClientSize = new System.Drawing.Size(248, 83);
      this.Controls.Add(this.checkBoxCase);
      this.Controls.Add(this.buttonClose);
      this.Controls.Add(this.buttonPrev);
      this.Controls.Add(this.buttonNext);
      this.Controls.Add(this.textBoxFind);
      this.Controls.Add(this.label1);
      this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
      this.MaximizeBox = false;
      this.MinimizeBox = false;
      this.Name = "Find";
      this.ShowIcon = false;
      this.Text = "Find";
      this.TopMost = true;
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    private System.Windows.Forms.Label label1;
    private System.Windows.Forms.TextBox textBoxFind;
    private System.Windows.Forms.Button buttonNext;
    private System.Windows.Forms.Button buttonPrev;
    private System.Windows.Forms.Button buttonClose;
    private System.Windows.Forms.CheckBox checkBoxCase;
  }
}