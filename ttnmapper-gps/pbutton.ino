//----------------------------------------//
//  pbutton.ino
//
//  created 03/06/2019
//  by Luiz Henrique Cassettari
//----------------------------------------//

#define BUTTON_PIN 2

static int button_i;

void button_setup()
{
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

boolean button_press()
{
  static boolean last;
  boolean now = digitalRead(BUTTON_PIN);
  boolean ret = (now == false & last == true);
  last = now;
  return ret;
}

boolean button_loop()
{
  if (button_press())
  {
    button_i++;
    return true;
  }
  return false;
}

int button_count()
{
  return button_i;
}
