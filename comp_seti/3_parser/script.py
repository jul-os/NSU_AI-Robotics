import time
import csv
import re

from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.support.wait import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC


def extract_storage(description):
    patterns = [
        r'(\d+)\s*GB(?:\s+of)?\s+(?:internal\s+)?storage',
        r'packs (\d+)\s*GB',
        r'(\d+)\s*GB(?:\s+of)?\s+internal',
        r'internal storage(?:\s+that)?\s+(?:can\s+)?(?:be\s+)?(\d+)\s*GB',
        r'(\d+)\s*GB.*(?:storage|memory)',
    ]
    for pattern in patterns:
        match = re.search(pattern, description, re.IGNORECASE)
        if match:
            return match.group(1) + " GB"
    return "-"


options = webdriver.FirefoxOptions()
driver = webdriver.Firefox(options=options)

driver.maximize_window()
driver.delete_all_cookies()

driver.get("https://www.demoblaze.com/")

login_button = driver.find_element(By.ID, "login2")
login_button.click()

wait = WebDriverWait(driver, 10)
username = wait.until(EC.element_to_be_clickable((By.ID, "loginusername")))
username.send_keys("scrypt_user_test")
username.send_keys(Keys.RETURN)

password = driver.find_element(By.ID, "loginpassword")
password.send_keys("secret!")
password.send_keys(Keys.RETURN)

login_button = driver.find_element(By.XPATH, "//button[text()='Log in']")
login_button.click()

wait.until(EC.invisibility_of_element_located((By.ID, "logInModal")))
wait.until(EC.presence_of_element_located((By.ID, "nameofuser")))

phone_category = wait.until(EC.element_to_be_clickable((By.XPATH, "//a[@id='itemc' and contains(text(), 'Phones')]")))
phone_category.click()
time.sleep(2)

product_cards = driver.find_elements(By.CSS_SELECTOR, ".card.h-100")

with open('phones.csv', 'w', newline='', encoding='utf-8') as file:
    writer = csv.writer(file)
    writer.writerow(['Название', 'Цена','Герцовка процессора','Объем оперативной памяти','Объем внутренней памяти'])
    for card in product_cards:
        try:
            name = card.find_element(By.CLASS_NAME, "card-title").text
            price = card.find_element(By.TAG_NAME, "h5").text
            description = card.find_element(By.CLASS_NAME, "card-text").text

            processor_match = re.search(r'(\d+\.?\d*)\s*GHz', description)
            processor = processor_match.group(1) + " GHz" if processor_match else "-"
            
            ram_match = re.search(r'(\d+)\s*GB(?:\s+of)?\s+RAM', description, re.IGNORECASE)
            ram = ram_match.group(1) + " GB" if ram_match else "-"
            
            storage = extract_storage(description)
            writer.writerow([name, price, processor, ram, storage])
        except Exception as e:
            print(f"   Ошибка: {e}")

time.sleep(2)
driver.close()