using System;
using System.Collections.Generic;
using System.Text;

namespace XabslEditor
{
  public class ErrorEntry
  {
    public string file;
    public int line;
    public string wholeLine;
    public string errorNumber;
    public string errorDescription;
    public string errorSample;
  }

  /// <summary>
  /// Represents a list with occured errors and information about then
  /// </summary>
  public class ErrorList
  {

    List<ErrorEntry> _errors;

    /// <summary>
    /// A list of all found errors
    /// </summary>
    public List<ErrorEntry> Errors
    {
      get { return _errors; }
    }

    enum State { information, lineString, marker };

    /// <summary>
    /// Constructor, will init the list by help of the given string
    /// </summary>
    /// <param name="compilerOutput">the string </param>
    public ErrorList(string compilerOutput)
    {
      _errors = new List<ErrorEntry>();

      State curState = State.information;
      string lastLine = "";
      
      // do some parsing with each line
      string[] lineSeperator = new string[] { "\r\n", "\n", "\r" };
      string[] lines = compilerOutput.Split(lineSeperator, StringSplitOptions.RemoveEmptyEntries);
      for (int l = 0; l < lines.Length; l++)
      {
        if (curState == State.information)
        {
          string[] sep1 = new string[] { ": " };
          string[] firstSplit = lines[l].Split(sep1, StringSplitOptions.None);

          // may not be an error-output
          if (firstSplit.Length == 3)
          {
            ErrorEntry entry = new ErrorEntry();

            // second contains error-number as string, third an description
            entry.errorNumber = firstSplit[1];
            entry.errorDescription = firstSplit[2];

            // the file and the line number are both in the first array-element
            int begin = firstSplit[0].LastIndexOf('(');
            int end = firstSplit[0].LastIndexOf(')');

            if (begin > -1 && end > -1 && begin < firstSplit[0].Length - 1 && begin < end)
            {
              entry.file = firstSplit[0].Remove(begin);
              string number = firstSplit[0].Substring(begin + 1, end - begin - 1);
              try
              {
                int converted = Int32.Parse(number);
                entry.line = converted;
              }
              catch
              {
                entry.line = -1;
              }
            }
            else
            {
              entry.file = "";
              entry.line = -1;
            }

            _errors.Add(entry);
            curState = State.lineString;
          }
        }
        else if (curState == State.lineString)
        {
          lastLine = lines[l];
          curState = State.marker;
        }
        else if (curState == State.marker)
        {
          string marks = lines[l];
          // find the ^^^-Block
          int begin = marks.IndexOf('^');
          int end = marks.LastIndexOf('^');

          if (begin > -1 && end > -1 && begin <= end && begin < lastLine.Length && end < lastLine.Length)
          {
            string sample = lastLine.Substring(begin, end - begin + 1);
            if(_errors.Count > 0)
            {
              _errors[_errors.Count - 1].errorSample = sample;
              _errors[_errors.Count - 1].wholeLine = lastLine;

            }
          }

          curState = State.information;

        }
      }
    }
  }
}
