VERSION 5.00
Begin VB.Form Form1 
   Caption         =   "Form1"
   ClientHeight    =   3090
   ClientLeft      =   60
   ClientTop       =   450
   ClientWidth     =   4680
   LinkTopic       =   "Form1"
   ScaleHeight     =   3090
   ScaleWidth      =   4680
   StartUpPosition =   3  'Windows Default
   Begin VB.TextBox Text1 
      Height          =   855
      Left            =   120
      TabIndex        =   0
      Text            =   "$PSRF100,0,9600,8,1,0*0C"
      Top             =   120
      Width           =   4335
   End
   Begin VB.Label Label1 
      Caption         =   "Label1"
      Height          =   615
      Left            =   120
      TabIndex        =   1
      Top             =   1920
      Width           =   4215
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
Private Sub Text1_Change()
  Dim s As String
  Dim dollar As Integer, star As Integer, i As Integer
  Dim sum As Integer
  s = Text1.Text
  dollar = InStr(s, "$")
  star = InStr(s, "*")
  Debug.Print dollar
  Debug.Print star
  If dollar > 0 And star > 0 Then
    s = Mid(s, dollar + 1, star - dollar - 1)
    Debug.Print s
    For i = 1 To Len(s)
      sum = sum Xor Asc(Mid(s, i, 1))
    Next
  End If
  If sum < 16 Then Label1.Caption = "0" Else Label1.Caption = ""
  Label1.Caption = Label1.Caption + Hex(sum)
End Sub
