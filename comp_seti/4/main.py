import asyncio
import os
import uvicorn

from contextlib import asynccontextmanager
from fastapi import FastAPI

from parser import Parser
from db import Database

parser = Parser(headless=True)
database = Database()


@asynccontextmanager
async def lifespan(app: FastAPI):
    yield
    await asyncio.to_thread(database.close)
    await asyncio.to_thread(parser.close)


app = FastAPI(lifespan=lifespan)


@app.put("/parse")
def parse_and_save():
    try:
        data = parser.parse_site(max_news=20, output_file="ura_news_data.csv")
        database.save_to_db(data)
        return {"message": "Парсинг и сохранение в базу данных успешно завершены."}
    except Exception as e:
        return {"error": f"Ошибка при парсинге или сохранении: {e}"}


@app.get("/data")
def get_data():
    try:
        data = database.get_all_data()
        return [
            {
                "url": item[0],
                "date": item[1],
                "time": item[2],
                "title": item[3],
                "author": item[4],
                "parsed_at": item[5],
            }
            for item in data
        ]

    except Exception as e:
        return {"error": f"Ошибка при получении данных из базы данных: {e}"}


@app.get("/clean_db")
def clean_db():
    database.clean_db()
    return {"status": "success"}


@app.put("/shutdown")
def shutdown_server():
    try:
        parser.close()
        database.close()
    except:
        pass

    print("Сервер завершает работу...")
    os._exit(0)


if __name__ == "__main__":
    uvicorn.run(app)
