/*
    Copyright (c) 2014 RedBearLab, All rights reserved.
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.
    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
    See the GNU Lesser General Public License for more details.
    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
/*
    This example show how to use timer task.
*/

static app_timer_id_t                   task1, task2; 

static uint8_t count=0;

void m_task1_handle(void * p_context)
{
    Serial.println("handle1 ");  
    count++;
    if(count > 20)
        APP_ERROR_CHECK( app_timer_start(task2, APP_TIMER_TICKS(50, 0), NULL) ); 
}

void m_task2_handle(void * p_context)
{
    Serial.println("handle2 ");    
    count=0;
}

void setup() {
    // put your setup code here, to run once:
    pinMode(D13, OUTPUT);
    Serial.begin(9600);
    Serial.println("Ticker Task Test ");    

    APP_ERROR_CHECK( app_timer_create(&task1,APP_TIMER_MODE_REPEATED, m_task1_handle) );
    APP_ERROR_CHECK( app_timer_create(&task2,APP_TIMER_MODE_SINGLE_SHOT, m_task2_handle) );
    
    APP_ERROR_CHECK( app_timer_start(task1, APP_TIMER_TICKS(1000, 0), NULL) );
    
}

void loop() {
    // put your main code here, to run repeatedly:
    digitalWrite(D13,HIGH);
    delay(500);
    digitalWrite(D13, LOW);
    delay(500);
}

