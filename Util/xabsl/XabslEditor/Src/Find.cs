using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace XabslEditor
{

  public partial class Find : Form
  {
    Scintilla.ScintillaControl _scintilla;

    public Find(Scintilla.ScintillaControl control)
    {
      InitializeComponent();

      _scintilla = control;

      if (_scintilla.SelTextSize > 0)
      {
        textBoxFind.Text = _scintilla.SelText;
      }

      // reset
      _scintilla.GotoPos(0);
      _scintilla.SearchAnchor();

    }

    private void buttonNext_Click(object sender, EventArgs e)
    {
      int flag = 0;
      if (checkBoxCase.Checked)
      {
        flag += (int)Scintilla.Enums.FindOption.matchcase;
      }
      int pos = _scintilla.SearchNext(flag, textBoxFind.Text);
      if (pos == -1)
      {
        MessageBox.Show("not found");
      }
      else if( pos > -1)
      {
        _scintilla.GotoPos(pos + 1);
        _scintilla.SearchAnchor();
        _scintilla.SetSel(pos, pos + textBoxFind.Text.Length);

      }
    }

    private void buttonClose_Click(object sender, EventArgs e)
    {
      this.Close();
    }

    private void buttonPrev_Click(object sender, EventArgs e)
    {
      int flag = 0;
      if (checkBoxCase.Checked)
      {
        flag += (int) Scintilla.Enums.FindOption.matchcase;
      }
      int pos = _scintilla.SearchPrev(flag, textBoxFind.Text);
      if (pos == -1)
      {
        MessageBox.Show("not found");
      }
      else if (pos > -1)
      {
        _scintilla.GotoPos(pos - 1);
        _scintilla.SearchAnchor();
        _scintilla.SetSel(pos, pos + textBoxFind.Text.Length);

      }
    }

    private void textBoxFind_TextChanged(object sender, EventArgs e)
    {
      // reset
      _scintilla.GotoPos(0);
      _scintilla.SearchAnchor();
    }
  }
}