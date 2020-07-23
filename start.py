import requests
from bs4 import BeautifulSoup
from selenium import webdriver
import urllib.request
import time
from selenium import webdriver
from webdriver_manager.chrome import ChromeDriverManager

browser = webdriver.Chrome(ChromeDriverManager().install())

xpath = '//*[@id="login"]/center/table/tbody/tr[3]/td[2]/button'

team16_path = '//*[@id="_in-cse"]/li[18]'

node16_path = '//*[@id="_in-cse_CAE780981258"]/li[2]'

instance_path = '//*[@id="_in-cse_cnt-395057765"]/li'

data_patj = '//*[@id="cont"]'
URL = 'http://onem2m.iiit.ac.in:443/webpage/welcome/index.html?context=/~&cseId=in-cse'
f1 = open('data16.txt', 'a')
try:
    browser.get(URL)
    button = browser.find_elements_by_xpath(xpath)
    # print(button)
    button[0].click()
    time.sleep(4)
    button = browser.find_elements_by_xpath(team16_path)
    print(button)
    button[0].click()
    time.sleep(4)
    button = browser.find_elements_by_xpath(node16_path)
    button[0].click()
    time.sleep(4)
    p = 1
    for i in range(1, 4350):
        button = browser.find_elements_by_xpath(
            instance_path + '[' + str(i) + ']')
        button[0].click()
        # time.sleep(.0005)
        html_source = browser.page_source
        soup = BeautifulSoup(html_source, 'html5lib')
        table = soup.find('table', attrs={'class': 'bordered'})
        l = len(table.findAll('tr'))
        cells = table.findAll('tr')[l-1].findAll('td')
        data = cells[1].string
        print(data)
        f1.write(str(data) + ' ')
        cells = table.findAll('tr')[l-5].findAll('td')
        data = cells[1].string
        print(data)
        f1.write(str(data) + '\n')

    browser.quit()
except Exception as e:
    print("error occured")
    print(str(e))
    exit()
print('no error')
f = open('html.html', 'w')
f.write(str(soup))
f.close()