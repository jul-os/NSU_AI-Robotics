# parser.py
import time
import csv
import re
import random
import undetected_chromedriver as uc
import os

from selenium.webdriver.common.by import By
from selenium.webdriver.support.wait import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from selenium_stealth import stealth
from datetime import datetime


class Parser:
    def __init__(self, headless=False):
        options = uc.ChromeOptions()

        if headless:
            options.add_argument("--headless=new")

        options.add_argument("--no-sandbox")
        options.add_argument("--disable-setuid-sandbox")
        options.add_argument("--disable-gpu")
        options.add_argument("--disable-dev-shm-usage")
        options.add_argument("--window-size=1920,1080")
        options.add_argument("--disable-blink-features=AutomationControlled")

        options.add_argument(
            "user-agent=Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 "
            "(KHTML, like Gecko) Chrome/143.0.0.0 Safari/537.36"
        )

        self.driver = uc.Chrome(options=options)

        stealth(
            self.driver,
            languages=["ru-RU", "ru"],
            vendor="Google Inc.",
            platform="Linux x86_64",
            webgl_vendor="Google Inc.",
            fix_hairline=True,
        )

        self.driver.maximize_window()
        self.wait = WebDriverWait(self.driver, 20)
        self.parsed_urls = set()

    def get_links(self, max_news=20):
        # Новости хранятся в контейнере ".aside-news_aside-news__list__knjHA",
        # а каждая карточка - это <li> внутри этого контейнера
        links = []

        news_container = None
        news_container = self.wait.until(
            EC.presence_of_element_located(
                (By.CSS_SELECTOR, ".aside-news_aside-news__list__knjHA")
            )
        )

        if not news_container:
            print("Контейнер с новостями не найден!")
            return []

        all_items = news_container.find_elements(By.TAG_NAME, "li")

        for i, item in enumerate(all_items[:max_news], 1):
            try:
                link = item.find_element(By.TAG_NAME, "a").get_attribute("href")
                if link and "/news/" in link:
                    # Если сслка относительная добавляем домен чтобы драйвер мог корректно открыть страницу
                    if link.startswith("/"):
                        link = "https://ura.news" + link
                    if link not in self.parsed_urls:
                        links.append(link)
            except Exception as e:
                continue

        print("Найдено ссылок на новости:", len(links))
        return links

    def get_text(self, driver, selector):
        try:
            elem = driver.find_element(By.CSS_SELECTOR, selector)
            text = elem.text.strip()
            return text
        except:
            return ""

    def parse_datetime(self, date_text):
        if not date_text:
            return "", ""
        pattern = r"(\d{1,2}\s+\w+\s+\d{4})\s+в\s+(\d{2}:\d{2})"
        match = re.search(pattern, date_text)
        if match:
            return match.group(1).strip(), match.group(2).strip()
        return date_text, ""

    def parse_news_item(self, article_url):
        if article_url in self.parsed_urls:
            return None
        self.parsed_urls.add(article_url)

        try:
            self.driver.get(article_url)
            time.sleep(random.uniform(2, 4))

            date_selector = "div.date_news-publication-date__gX9cp"
            title_selector = "h1.detail_news-publication-detail__title__TAtmm"
            author_selector = "span.authors_news-publication-authors__full-name__L2WC2"

            date = self.get_text(self.driver, date_selector)
            pub_date, pub_time = self.parse_datetime(date)
            title = self.get_text(self.driver, title_selector)
            author = self.get_text(self.driver, author_selector)
            return {
                "url": article_url,
                "date": pub_date,
                "time": pub_time,
                "title": title,
                "author": author if author else "Не указан",
                "parsed_at": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
            }
        except Exception as e:
            print(f"Ошибка при парсинге {article_url}: {e}")
            return None

    def save_to_csv(self, data_list, filename="ura_news_data.csv"):
        file_exists = os.path.isfile(filename)
        if not data_list:
            return
        fieldnames = ["url", "date", "time", "title", "author", "parsed_at"]
        with open(filename, mode="a", encoding="utf-8", newline="") as csvfile:
            writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
            if not file_exists:
                writer.writeheader()
            for data in data_list:
                writer.writerow(data)

    def parse_site(self, max_news=20, output_file="ura_news_data.csv"):
        self.driver.get("https://ura.news/")
        time.sleep(random.uniform(4, 7))
        try:
            consent = self.wait.until(
                EC.element_to_be_clickable(
                    (
                        By.CSS_SELECTOR,
                        "button.fc-button.fc-cta-consent.fc-primary-button",
                    )
                )
            )
            time.sleep(random.uniform(1, 2))
            consent.click()
        except:
            print("")

        news_links = self.get_links(max_news=max_news)
        parsed_data = []
        for link in news_links:
            result = self.parse_news_item(link)
            if result:
                parsed_data.append(result)
                print(f"Успешно спарсили: {link}")

        time.sleep(random.uniform(5, 12))

        if parsed_data:
            self.save_to_csv(parsed_data, output_file)

        return parsed_data

    def close(self):
        self.driver.quit()


if __name__ == "__main__":
    parser = Parser(headless=True)
    try:
        data = parser.parse_site(max_news=20, output_file="ura_news_data.csv")
    except Exception as e:
        print(f"Ошибка при парсинге сайта: {e}")
    finally:
        parser.close()
