from fastapi import FastAPI
from uvicorn import run
import multiprocessing

import api

if __name__ == '__main__':
    multiprocessing.freeze_support()
    run(api.app, host="0.0.0.0", port=36680, reload=False, workers=1)