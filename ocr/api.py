from fastapi import FastAPI
from pydantic import BaseModel
from rapidocr_onnxruntime import RapidOCR
from fastapi.middleware.cors import CORSMiddleware

engine = RapidOCR()
app = FastAPI()

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)


class Item(BaseModel):
    path: str


@app.post("/check")
async def check(item: Item):
    result, elapse = engine(item.path)
    if result:
        text: list[str] = [item[1] for item in result]
        print(text)
        return {"success": True, "result": text}

    return {"success": True, "result": None}
