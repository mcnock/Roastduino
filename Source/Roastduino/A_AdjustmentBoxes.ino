//----------------------------------------------------------------------------------------------------------------------------------------------------
// ADJUSTMENT          ADJUSTMENT          ADJUSTMENT          ADJUSTMENT          ADJUSTMENT          ADJUSTMENT          ADJUSTMENT          ADJUSTMENT
//----------------------------------------------------------------------------------------------------------------------------------------------------

void UpdateAdjustMentText()
{
  tft.setTextColor(WHITE, BLACK);  tft.setTextSize(1);
  tft.setCursor(225, AdjustmentDisplayTop + 25);  tft.println("New:"); tft.setCursor(260, AdjustmentDisplayTop + 25) ;   tft.println( MyBaseSetpoints[AdjustmentSetpoint]);
  tft.setCursor(225, AdjustmentDisplayTop + 55 ); tft.println("New:"); tft.setCursor(260, AdjustmentDisplayTop + 55);  tft.println( MySpanAccumulatedMinutes[AdjustmentSetpoint]);
}
void HideAdjustMentBoxes()
{
  AdjustmentSetpoint = 0;
  tft.fillRect(150, AdjustmentDisplayTop - 10, 150, 80, BLACK);
}

void DrawAdjustMentBoxes(int setpoint)
{
  // make the color selection boxes
  AdjustmentSetpoint = setpoint;

  tft.fillRect(150, AdjustmentDisplayTop - 10, 150, 80, BLACK);
  tft.drawRect(150, AdjustmentDisplayTop - 10, 150, 80, BLUE);

  tft.fillRect(180, AdjustmentDisplayTop      , 20,  20, GREEN); //TOP
  tft.fillRect(200, AdjustmentDisplayTop + 20 , 20, 20, GREEN); //RIGHT
  tft.fillRect(180, AdjustmentDisplayTop + 40 , 20, 20, GREEN); //BOTTOM
  tft.fillRect(160, AdjustmentDisplayTop + 20 , 20, 20, GREEN); //LEFT

  tft.fillRect(280, AdjustmentDisplayTop - 10 , 20, 20,  BLUE); //LEFT
  tft.drawLine(280, AdjustmentDisplayTop + 10 , 300 ,  AdjustmentDisplayTop - 10, BLACK);
  tft.drawLine(280, AdjustmentDisplayTop - 10 , 300 ,  AdjustmentDisplayTop + 10, BLACK);

  tft.setTextColor(WHITE, BLACK);  tft.setTextSize(2);
  tft.setCursor(184, AdjustmentDisplayTop + 23 ); tft.println(setpoint);

  tft.setTextColor(WHITE, BLACK);  tft.setTextSize(1);
  tft.setCursor(225, AdjustmentDisplayTop + 15);  tft.println("Temp:"); tft.setCursor(260, AdjustmentDisplayTop + 15) ;   tft.println( MyBaseSetpoints[AdjustmentSetpoint]);
  tft.setCursor(225, AdjustmentDisplayTop + 40 ); tft.println("Mins:"); tft.setCursor(260, AdjustmentDisplayTop + 40);  tft.println( MySpanAccumulatedMinutes[AdjustmentSetpoint]);

}

